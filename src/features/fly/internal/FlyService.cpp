// [0.6.0 §12 2] specs/features/0.6.0-fly.md
// [0.6.0 5.11.7] specs/features/0.6.0-fly.md
#include <kgc/fly/FlyService.h>
#include "Events.h"
#include "FlyChecks.h"
#include "FlyRunner.h"
#include "FlyText.h"
#include "GoToTracker.h"
#include <kgc/app/geo/Altitude.h>
#include <kgc/app/geo/Geodesy.h>
#include <kgc/app/log/Log.h>
#include <kgc/fly/Errors.h>
#include <kgc/params/ParamService.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <kgc/vehicle/safety/GuidedLimits.h>
#include <kgc/vehicle/states/VehicleStates.h>
#include <QAbstractItemModel>
#include <algorithm>
#include <cmath>
#include <limits>
namespace kgc::fly {
using kgc::app::geo::isKnownPosition;
using kgc::vehicle::firmware::CommandStep;
using kgc::vehicle::firmware::TakeoffKind;
using kgc::vehicle::safety::GuidedCheck;
using kgc::vehicle::states::VehicleStates;
FlyService::FlyService(kgc::vehicle::registry::Vehicle& vehicle, kgc::vehicle::safety::ArmedPolicy& policy, QObject* parent)
    : QObject(parent), vehicle_(vehicle), policy_(policy), params_(kgc::params::ParamService::of(&vehicle)),
      runner_(new FlyRunner(vehicle, this)), tracker_(new GoToTracker(*vehicle.states(), this))
{
    auto* states = vehicle_.states();
    for (auto signal : {&VehicleStates::linkChanged, &VehicleStates::armingChanged, &VehicleStates::positionChanged,
                        &VehicleStates::capabilitiesChanged, &VehicleStates::flightChanged, &VehicleStates::navTargetChanged})
        connect(states, signal, this, &FlyService::refresh);
    if (params_) {
        // 파라미터 값(이륙 고도·선회 반지름·떠 있기)이 바뀌면 능력과 선회 원을 다시 봐요.
        auto* model = params_->params();
        connect(model, &QAbstractItemModel::dataChanged, this, &FlyService::refresh);
        connect(model, &QAbstractItemModel::modelReset, this, &FlyService::refresh);
        connect(model, &QAbstractItemModel::rowsInserted, this, &FlyService::refresh);
    }
    connect(runner_, &FlyRunner::finished, this, &FlyService::onRunnerFinished);
    connect(runner_, &FlyRunner::reasonArrived, this, [this](FlyActionKind kind, const QString& reason) {
        // ModeNotChanged로 5초 지켜보는 여기로 가기는 그 결과에 이유를 실어 보내므로 거기에만 넣어요(§12 고침 2).
        if (kind == FlyActionKind::GoTo && goToWatching_) {
            if (pendingReason_.isEmpty()) pendingReason_ = reason;
            return;
        }
        emit actionReason(kind, reason);
    });
    connect(tracker_, &GoToTracker::changed, this, [this] { emit targetChanged(); refresh(); });
    connect(tracker_, &GoToTracker::watchEnded, this, &FlyService::onWatchEnded);
    connect(tracker_, &GoToTracker::rejected, this, [this](const QString& reason) {
        emit actionFinished(FlyActionKind::GoTo, std::unexpected(make(FlyError::TargetRejected)), reason);
        refresh();
    });
    capabilities_ = capabilities();
    readiness_ = readiness();
    busy_ = runner_->busy();
    loiter_ = loiter();
}
FlyService::~FlyService() = default;
FlyService* FlyService::of(kgc::vehicle::registry::Vehicle* vehicle)
{
    return vehicle ? vehicle->findChild<FlyService*>(QString(), Qt::FindDirectChildrenOnly) : nullptr;
}
kgc::vehicle::registry::Vehicle& FlyService::vehicle() const { return vehicle_; }
FlyCapabilities FlyService::capabilities() const
{
    const auto& plugin = vehicle_.plugin();
    const auto* states = vehicle_.states();
    const int type = states->capabilities().vehicleType;
    const int vtol = states->flight().vtolState;
    FlyCapabilities caps;
    caps.vehicleClass = kgc::vehicle::firmware::vehicleClassOf(type, vtol);
    caps.shape = kgc::vehicle::firmware::flightShapeOf(caps.vehicleClass, vtol);
    caps.classKnown = !(plugin.vtolStateDecidesClass(type) && vtol < 0);
    caps.takeoff = plugin.takeoffKind(caps.vehicleClass);
    caps.takeoffAltitudeParam = plugin.takeoffAltitudeParam();
    caps.takeoffFixedAltitudeM = paramValue(caps.takeoffAltitudeParam);
    caps.land = !plugin.landSteps(caps.vehicleClass).isEmpty();
    caps.rtl = !plugin.rtlSteps(caps.vehicleClass).isEmpty();
    caps.pause = !plugin.pauseSteps(caps.vehicleClass, caps.shape, states->capabilities().firmwareVersion).isEmpty();
    caps.goTo = plugin.guidedMode(caps.vehicleClass).has_value();
    caps.loitersAtTarget = plugin.loitersAtTarget(caps.vehicleClass, caps.shape, paramValue(plugin.vtolHoverParam()));
    caps.modes = plugin.flightModes(caps.vehicleClass);
    caps.launchOnArmModes = plugin.launchOnArmModes(caps.vehicleClass);
    return caps;
}
FlyReadiness FlyService::readiness() const
{
    const auto* states = vehicle_.states();
    const auto link = states->link();
    const auto arming = states->arming();
    const auto position = states->position();
    return readinessOf({capabilities(), link.connected && !link.lost, arming.armed, states->flight().landedState,
        isKnownPosition(position.latDeg, position.lonDeg),
        isKnownPosition(position.homeLatDeg, position.homeLonDeg) && std::isfinite(position.homeAltAmslM),
        runner_->busy(), arming.modeId});
}
bool FlyService::busy() const { return runner_->busy(); }
void FlyService::arm() { start(FlyActionKind::Arm, vehicle_.plugin().armSteps()); }
void FlyService::disarm() { start(FlyActionKind::Disarm, vehicle_.plugin().disarmSteps()); }
void FlyService::emergencyStop() { start(FlyActionKind::EmergencyStop, vehicle_.plugin().emergencyStopSteps()); }
void FlyService::takeoff(double heightM)
{
    const auto& plugin = vehicle_.plugin();
    const auto caps = capabilities();
    double value = std::numeric_limits<double>::quiet_NaN();
    if (caps.takeoff == TakeoffKind::WithAltitude) {
        if (checkGuided(kgc::vehicle::safety::guidedLimits(), 0, heightM) != GuidedCheck::Ok) {
            finishLater(FlyActionKind::Takeoff, make(FlyError::AltitudeOutOfRange));
            return;
        }
        const auto position = vehicle_.states()->position();
        const auto converted = kgc::app::geo::heightIn(plugin.takeoffAltitudeFrame(caps.vehicleClass), heightM,
                                                       position.altRelM, position.altAmslM);
        if (!converted) { finishLater(FlyActionKind::Takeoff, make(FlyError::PositionUnknown)); return; }
        value = *converted;
    }
    start(FlyActionKind::Takeoff, plugin.takeoffSteps(caps.vehicleClass, value, vehicle_.states()->arming().armed));
}
void FlyService::land() { start(FlyActionKind::Land, vehicle_.plugin().landSteps(capabilities().vehicleClass)); }
void FlyService::rtl() { start(FlyActionKind::Rtl, vehicle_.plugin().rtlSteps(capabilities().vehicleClass)); }
void FlyService::pause()
{
    const auto caps = capabilities();
    const auto version = vehicle_.states()->capabilities().firmwareVersion;
    start(FlyActionKind::Pause, vehicle_.plugin().pauseSteps(caps.vehicleClass, caps.shape, version));
}
void FlyService::setMode(std::uint32_t customMode)
{
    const auto caps = capabilities();
    const auto found = std::find_if(caps.modes.cbegin(), caps.modes.cend(),
                                    [customMode](const auto& mode) { return mode.customMode == customMode; });
    if (found == caps.modes.cend() || !modeChoosable(*found, vehicle_.states()->flight().landedState)) {
        finishLater(FlyActionKind::SetMode, make(FlyError::NotReady));
        return;
    }
    start(FlyActionKind::SetMode, vehicle_.plugin().setModeSteps(caps.vehicleClass, customMode));
}
kgc::app::errors::Result<void> FlyService::checkGoTo(double latDeg, double lonDeg, double altitudeRelM) const
{
    if (!readiness().goTo || !isKnownPosition(latDeg, lonDeg)) return std::unexpected(make(FlyError::NotReady));
    switch (checkGuided(kgc::vehicle::safety::guidedLimits(), distanceToM(latDeg, lonDeg), altitudeRelM)) {
    case GuidedCheck::TooFar: return std::unexpected(make(FlyError::TooFar));
    case GuidedCheck::TooLow:
    case GuidedCheck::TooHigh: return std::unexpected(make(FlyError::AltitudeOutOfRange));
    case GuidedCheck::Ok: break;
    }
    const auto home = vehicle_.states()->position().homeAltAmslM;
    if (!kgc::app::geo::altitudeIn(vehicle_.plugin().goToAltitudeFrame(), altitudeRelM, home))
        return std::unexpected(make(FlyError::HomeUnknown));
    return {};
}
kgc::app::errors::Result<void> FlyService::goTo(double latDeg, double lonDeg, double altitudeRelM)
{
    if (const auto check = checkGoTo(latDeg, lonDeg, altitudeRelM); !check) return check;
    const auto& plugin = vehicle_.plugin();
    const auto cls = capabilities().vehicleClass;
    const auto home = vehicle_.states()->position().homeAltAmslM;
    const auto altitude = kgc::app::geo::altitudeIn(plugin.goToAltitudeFrame(), altitudeRelM, home);
    auto steps = plugin.goToSteps(cls, latDeg, lonDeg, *altitude);
    tracker_->begin(latDeg, lonDeg, altitudeRelM, plugin.guidedMode(cls).value_or(0));
    KGC_LOG_INFO(kCategory, FlyEvent::GoToSent, "Go-to sent: %1, %2, %3 m above home, sysid %4",
                 QString::number(latDeg, 'f', 7), QString::number(lonDeg, 'f', 7), lengthNumber(altitudeRelM), vehicle_.sysid());
    start(FlyActionKind::GoTo, std::move(steps));
    return {};
}
double FlyService::distanceToM(double latDeg, double lonDeg) const
{
    const auto position = vehicle_.states()->position();
    if (!isKnownPosition(position.latDeg, position.lonDeg)) return std::numeric_limits<double>::quiet_NaN();
    return kgc::app::geo::distanceMeters(position.latDeg, position.lonDeg, latDeg, lonDeg);
}
GoToTarget FlyService::target() const { return tracker_->target(); }
kgc::vehicle::firmware::LoiterCircle FlyService::loiter() const
{
    const auto state = tracker_->target().state;
    if (state != GoToState::Accepted && state != GoToState::Confirmed) return {};
    const auto& plugin = vehicle_.plugin();
    const auto caps = capabilities();
    return plugin.loiterCircle(caps.vehicleClass, caps.shape, vehicle_.states()->navTarget().orbitRadiusM,
                               paramValue(plugin.loiterRadiusParam()), paramValue(plugin.vtolHoverParam()));
}
void FlyService::refresh()
{
    const auto caps = capabilities();
    const auto ready = readiness();
    const bool busy = runner_->busy();
    const auto circle = loiter();
    if (caps != capabilities_) { capabilities_ = caps; emit capabilitiesChanged(); }
    // 바쁨은 readiness 칸이 같아도 상단바 기다림 표시가 바뀌므로 함께 알려요.
    if (ready != readiness_ || busy != busy_) { readiness_ = ready; busy_ = busy; emit readinessChanged(); }
    if (circle != loiter_) { loiter_ = circle; emit loiterChanged(); }
}
std::optional<double> FlyService::paramValue(const QString& name) const
{
    if (name.isEmpty() || !params_) return std::nullopt;
    const auto* param = params_->params()->find(name);
    if (!param || !std::isfinite(param->value)) return std::nullopt;
    return param->value;
}
void FlyService::start(FlyActionKind kind, QList<CommandStep> steps)
{
    const auto ready = readiness();
    bool allowed = false;
    switch (kind) {
    case FlyActionKind::Arm: allowed = ready.arm; break;
    case FlyActionKind::Disarm: allowed = ready.disarm; break;
    case FlyActionKind::EmergencyStop: allowed = ready.emergencyStop; break;
    case FlyActionKind::Takeoff: allowed = ready.takeoff; break;
    case FlyActionKind::Land: allowed = ready.land; break;
    case FlyActionKind::Rtl: allowed = ready.rtl; break;
    case FlyActionKind::Pause: allowed = ready.pause; break;
    case FlyActionKind::SetMode: allowed = ready.setMode; break;
    case FlyActionKind::GoTo: allowed = ready.goTo; break;
    }
    // 시동 끄기·비상 정지만 진행 중인 실행에 끼어들어요(§2 '비상 정지').
    const bool preempt = kind == FlyActionKind::EmergencyStop || kind == FlyActionKind::Disarm;
    using kgc::vehicle::safety::Decision;
    std::optional<FlyError> error;
    if (!allowed) error = FlyError::NotReady;
    else if (runner_->busy() && !preempt) error = FlyError::Busy;
    else if (policy_.check(kgc::app::job::OperationKind::FlyAction, kgc::vehicle::safety::VehicleScope::One(&vehicle_))
             == Decision::Deny) error = FlyError::PolicyDenied;
    else if (steps.isEmpty()) error = FlyError::Unsupported;
    if (error) {
        // 점선 표식이 남지 않게 여기로 가기 목표도 되돌려요.
        if (kind == FlyActionKind::GoTo) tracker_->fail();
        finishLater(kind, make(*error));
        return;
    }
    KGC_LOG_INFO(kCategory, FlyEvent::ActionStarted, "Fly action started: %1, sysid %2", actionName(kind), vehicle_.sysid());
    runner_->start(kind, std::move(steps), preempt);
    refresh();
}
void FlyService::finishLater(FlyActionKind kind, kgc::app::errors::ErrorCode error)
{
    // 부르는 쪽이 늘 나중에 결과를 받게 큐를 거쳐 내요.
    QMetaObject::invokeMethod(this, [this, kind, error] {
        emit actionFinished(kind, std::unexpected(error), QString{});
    }, Qt::QueuedConnection);
}
void FlyService::onRunnerFinished(FlyActionKind kind, kgc::app::errors::Result<void> result, const QString& reason)
{
    if (result) {
        KGC_LOG_INFO(kCategory, FlyEvent::ActionFinished, "Fly action finished: %1, sysid %2", actionName(kind), vehicle_.sysid());
    } else if (reason.isEmpty()) {
        KGC_LOG_WARN(kCategory, FlyEvent::ActionFailed, "Fly action failed: %1, %2", actionName(kind),
                     flyErrorName(FlyError(result.error().code)));
    } else {
        KGC_LOG_WARN(kCategory, FlyEvent::ActionFailed, "Fly action failed: %1, %2, vehicle: %3", actionName(kind),
                     flyErrorName(FlyError(result.error().code)), reason);
    }
    // ArduPlane은 모드를 못 바꿔도 목표를 받을 수 있어 5초 더 보고 watchEnded에서 결과를 내요(§2).
    const bool modeMissed = kind == FlyActionKind::GoTo && !result && result.error() == make(FlyError::ModeNotChanged);
    if (kind == FlyActionKind::GoTo) {
        if (result) tracker_->accept();
        else if (modeMissed) { tracker_->watch(); pendingReason_ = reason; goToWatching_ = true; }
        else tracker_->fail();
    } else if (result && kind != FlyActionKind::Arm && kind != FlyActionKind::SetMode) {
        // PX4 일시정지도 Hold라서 다른 동작이 성공하면 목표를 따로 지워요.
        tracker_->clear(QStringLiteral("Action"));
    }
    if (!modeMissed) emit actionFinished(kind, result, reason);
    refresh();
}
void FlyService::onWatchEnded(bool kept)
{
    const auto guided = vehicle_.plugin().guidedMode(capabilities().vehicleClass);
    const auto modeId = vehicle_.states()->arming().modeId;
    if (kept && guided && modeId == static_cast<qint64>(*guided)) {
        // 모드는 바뀌었는데 HEARTBEAT가 늦어 ModeNotChanged로 끝났던 경우예요(성공, 토스트 없음).
        KGC_LOG_INFO(kCategory, FlyEvent::ActionFinished, "Fly action finished: %1, sysid %2",
                     actionName(FlyActionKind::GoTo), vehicle_.sysid());
        emit actionFinished(FlyActionKind::GoTo, {}, QString{});
    } else {
        const auto error = make(kept ? FlyError::TargetWithoutMode : FlyError::ModeNotChanged);
        emit actionFinished(FlyActionKind::GoTo, std::unexpected(error), pendingReason_);
    }
    pendingReason_.clear();
    goToWatching_ = false;
    refresh();
}
}
