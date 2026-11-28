// [0.6.0 §12 2] specs/features/0.6.0-fly.md
// [0.6.0 5.11.8] specs/features/0.6.0-fly.md
#include "FlyController.h"
#include "Constants.h"
#include "FlyChecks.h"
#include "FlyText.h"
#include <kgc/app/geo/Geodesy.h>
#include <kgc/fly/Errors.h>
#include <kgc/safety/Settings.h>
#include <kgc/vehicle/safety/GuidedLimits.h>
#include <QCoreApplication>
#include <algorithm>
#include <cmath>
namespace kgc::fly {
using kgc::vehicle::firmware::TakeoffKind;
FlyController::FlyController(kgc::vehicle::registry::VehicleRegistry& vehicles, kgc::ui::TopbarSlot& topbar)
    : QObject(&vehicles), vehicles_(vehicles), topbar_(topbar), takeoffAltitude_(kDefaultTakeoffAltitudeM)
{
    Q_ASSERT(!instance_);
    instance_ = this;
    namespace settings = kgc::safety::settings;
    settings::onGuidedMaxDistanceMChanged(this, [this](double) { emit limitsChanged(); });
    settings::onGuidedMinAltitudeMChanged(this, [this](double) { emit limitsChanged(); });
    settings::onGuidedMaxAltitudeMChanged(this, [this](double) { emit limitsChanged(); });
    connect(&vehicles_, &kgc::vehicle::registry::VehicleRegistry::activeChanged, this, &FlyController::rebind);
    rebind();
}
FlyController* FlyController::create(QQmlEngine* engine, QJSEngine*)
{
    Q_ASSERT(instance_ && instance_->thread() == engine->thread());
    QQmlEngine::setObjectOwnership(instance_, QQmlEngine::CppOwnership);
    return instance_;
}
void FlyController::rebind()
{
    for (const auto& connection : std::as_const(connections_)) disconnect(connection);
    connections_.clear();
    appendableKind_.reset();
    auto* vehicle = vehicles_.active();
    service_ = FlyService::of(vehicle);
    if (service_) {
        using kgc::vehicle::states::VehicleStates;
        auto* states = vehicle->states();
        for (auto signal : {&VehicleStates::armingChanged, &VehicleStates::capabilitiesChanged, &VehicleStates::flightChanged})
            connections_.append(connect(states, signal, this, &FlyController::refreshState));
        connections_.append(connect(states, &VehicleStates::positionChanged, this, &FlyController::refreshCoordinate));
        connections_.append(connect(service_, &FlyService::capabilitiesChanged, this, [this] { applyHidden(); refreshState(); }));
        connections_.append(connect(service_, &FlyService::readinessChanged, this, [this] {
            emit readinessChanged();
            refreshState();
        }));
        connections_.append(connect(service_, &FlyService::targetChanged, this, &FlyController::targetChanged));
        connections_.append(connect(service_, &FlyService::loiterChanged, this, &FlyController::loiterChanged));
        connections_.append(connect(service_, &FlyService::actionFinished, this, &FlyController::showFailure));
        connections_.append(connect(service_, &FlyService::actionFinished, this, &FlyController::refreshState));
        connections_.append(connect(service_, &FlyService::actionReason, this, &FlyController::appendReason));
    }
    emit vehicleChanged();
    applyHidden();
    refreshState();
    emit readinessChanged();
    refreshCoordinate();
    emit targetChanged();
    emit loiterChanged();
}
QVariantList FlyController::stateSnapshot() const
{
    return {modeText(), currentMode(), modes(), armed(), busy(), takeoffKind(), takeoffFixedText(), goToHint()};
}
void FlyController::refreshState()
{
    // HEARTBEAT마다 오는 알림을 그대로 넘기지 않고, 보이는 값의 모음이 바뀐 때만 알려요.
    auto now = stateSnapshot();
    if (now == state_) return;
    state_ = std::move(now);
    emit stateChanged();
}
void FlyController::refreshCoordinate()
{
    const auto now = vehicleCoordinate();
    if (now == coordinate_) return;
    coordinate_ = now;
    emit vehicleCoordinateChanged();
}
void FlyController::applyHidden()
{
    // 종류·펌웨어가 지원하지 않는 단추는 숨겨요(R-Q12). 종류를 모르면 넷 모두 보이고 흐림은 readiness가 정해요.
    const auto caps = service_ ? service_->capabilities() : FlyCapabilities{};
    const bool decide = service_ && caps.classKnown;
    const auto hide = [this](const char* id, bool hidden) {
        const auto result = topbar_.setHidden(QString::fromLatin1(id), hidden);
        Q_ASSERT(result.has_value());
    };
    hide("fly.takeoff", decide && caps.takeoff == TakeoffKind::None);
    hide("fly.land", decide && !caps.land);
    hide("fly.rtl", decide && !caps.rtl);
    hide("fly.pause", decide && !caps.pause);
}
void FlyController::showFailure(FlyActionKind kind, const kgc::app::errors::Result<void>& result, const QString& reason)
{
    // 늦은 이유는 바로 앞 결과의 실패 토스트에만 붙여요. 다른 결과가 오면 덧붙일 기회는 사라져요(§12 고침 2).
    appendableKind_.reset();
    if (result || result.error() == make(FlyError::Preempted)) return;
    auto text = result.error() == make(FlyError::TargetWithoutMode)
        ? errorText(result.error()) : actionFailureText(kind) + QStringLiteral(". ") + errorText(result.error());
    if (!reason.isEmpty()) {
        text += QStringLiteral("\n") + QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "기체: %1")).arg(reason);
    } else {
        appendableKind_ = kind;
        appendableText_ = text;
    }
    emit toast(text);
}
void FlyController::appendReason(FlyActionKind kind, const QString& reason)
{
    // 그 동작의 실패 토스트에 이유 줄이 없을 때만 한 번 덧붙여요. 늦은 이유는 실패 뒤 1초(kReasonGrace) 안에만 오므로
    // 6초 동안 떠 있는 그 토스트는 아직 열려 있어요. 그 사이 다른 결과의 토스트가 나왔으면 기록이 비어 아무것도 하지 않아요.
    if (appendableKind_ != kind) return;
    appendableKind_.reset();
    emit toast(appendableText_ + QStringLiteral("\n")
               + QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "기체: %1")).arg(reason));
}
bool FlyController::isActive(int sysid) const { return service_ && service_->vehicle().sysid() == sysid; }
bool FlyController::hasVehicle() const { return service_; }
int FlyController::sysid() const { return service_ ? service_->vehicle().sysid() : -1; }
QString FlyController::vehicleText() const
{
    return service_ ? QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "기체 %1")).arg(sysid())
                    : QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "기체 없음"));
}
QString FlyController::modeText() const
{
    const auto none = QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "모드: —"));
    if (!service_) return none;
    const auto caps = service_->capabilities();
    if (!caps.classKnown) return none;
    // 그 종류의 모드 목록 이름을 먼저 써요(고정익 type 기체에 일반 Plane과 쿼드플레인 중 틀린 쪽 이름이 보이지 않게).
    const auto arming = service_->vehicle().states()->arming();
    const auto found = std::find_if(caps.modes.cbegin(), caps.modes.cend(),
        [&arming](const auto& mode) { return static_cast<qint64>(mode.customMode) == arming.modeId; });
    const auto name = found != caps.modes.cend() ? found->name : arming.modeName;
    if (name.isEmpty()) return none;
    return QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "모드: %1")).arg(name);
}
double FlyController::currentMode() const
{
    return service_ ? static_cast<double>(service_->vehicle().states()->arming().modeId) : -1;
}
QVariantList FlyController::modes() const
{
    QVariantList result;
    if (!service_) return result;
    const int landed = service_->vehicle().states()->flight().landedState;
    for (const auto& mode : service_->capabilities().modes)
        result.append(QVariantMap{{"customMode", static_cast<double>(mode.customMode)}, {"name", mode.name},
                                  {"choosable", modeChoosable(mode, landed)}});
    return result;
}
bool FlyController::armed() const { return service_ && service_->vehicle().states()->arming().armed; }
bool FlyController::busy() const { return service_ && service_->busy(); }
int FlyController::takeoffKind() const { return service_ ? static_cast<int>(service_->capabilities().takeoff) : 0; }
QString FlyController::takeoffFixedText() const
{
    if (!service_) return {};
    const auto caps = service_->capabilities();
    if (caps.takeoffFixedAltitudeM)
        return QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "기체 설정 %1(%2 m)까지 상승"))
            .arg(caps.takeoffAltitudeParam, lengthNumber(*caps.takeoffFixedAltitudeM));
    return QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "기체 설정 %1까지 상승")).arg(caps.takeoffAltitudeParam);
}
QString FlyController::goToHint() const
{
    return service_ && service_->capabilities().loitersAtTarget
        ? QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "도착 후 목표 지점 선회"))
        : QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "도착 후 제자리 정지"));
}
bool FlyController::canArm() const { return service_ && service_->readiness().arm; }
bool FlyController::canDisarm() const { return service_ && service_->readiness().disarm; }
bool FlyController::canEmergencyStop() const { return service_ && service_->readiness().emergencyStop; }
bool FlyController::canTakeoff() const { return service_ && service_->readiness().takeoff; }
bool FlyController::canLand() const { return service_ && service_->readiness().land; }
bool FlyController::canRtl() const { return service_ && service_->readiness().rtl; }
bool FlyController::canPause() const { return service_ && service_->readiness().pause; }
bool FlyController::canSetMode() const { return service_ && service_->readiness().setMode; }
bool FlyController::canGoTo() const { return service_ && service_->readiness().goTo; }
double FlyController::takeoffAltitude() const
{
    const auto limits = kgc::vehicle::safety::guidedLimits();
    return std::clamp(takeoffAltitude_, limits.minAltitudeM, limits.maxAltitudeM);
}
void FlyController::setTakeoffAltitude(double value)
{
    if (value == takeoffAltitude_) return;
    takeoffAltitude_ = value;
    emit takeoffAltitudeChanged();
}
double FlyController::guidedMaxDistanceM() const { return kgc::safety::settings::guidedMaxDistanceM(); }
double FlyController::guidedMinAltitudeM() const { return kgc::safety::settings::guidedMinAltitudeM(); }
double FlyController::guidedMaxAltitudeM() const { return kgc::safety::settings::guidedMaxAltitudeM(); }
void FlyController::setGuidedMaxDistanceM(double value) { kgc::safety::settings::setGuidedMaxDistanceM(value); }
void FlyController::setGuidedMinAltitudeM(double value) { kgc::safety::settings::setGuidedMinAltitudeM(value); }
void FlyController::setGuidedMaxAltitudeM(double value) { kgc::safety::settings::setGuidedMaxAltitudeM(value); }
double FlyController::guidedMaxDistanceMMin() const { return kgc::safety::settings::guidedMaxDistanceMMin(); }
double FlyController::guidedMaxDistanceMMax() const { return kgc::safety::settings::guidedMaxDistanceMMax(); }
double FlyController::guidedMinAltitudeMMin() const { return kgc::safety::settings::guidedMinAltitudeMMin(); }
double FlyController::guidedMinAltitudeMMax() const { return kgc::safety::settings::guidedMinAltitudeMMax(); }
double FlyController::guidedMaxAltitudeMMin() const { return kgc::safety::settings::guidedMaxAltitudeMMin(); }
double FlyController::guidedMaxAltitudeMMax() const { return kgc::safety::settings::guidedMaxAltitudeMMax(); }
double FlyController::guidedAltitudeFrom() const { return kgc::vehicle::safety::guidedLimits().minAltitudeM; }
double FlyController::guidedAltitudeTo() const { return kgc::vehicle::safety::guidedLimits().maxAltitudeM; }
QGeoCoordinate FlyController::vehicleCoordinate() const
{
    if (!service_) return {};
    const auto position = service_->vehicle().states()->position();
    if (!kgc::app::geo::isKnownPosition(position.latDeg, position.lonDeg)) return {};
    return QGeoCoordinate(position.latDeg, position.lonDeg);
}
bool FlyController::targetVisible() const { return service_ && service_->target().state != GoToState::None; }
bool FlyController::targetPending() const { return service_ && service_->target().state == GoToState::Sending; }
QGeoCoordinate FlyController::targetCoordinate() const
{
    if (!targetVisible()) return {};
    const auto target = service_->target();
    return QGeoCoordinate(target.latDeg, target.lonDeg);
}
QString FlyController::targetText() const
{
    if (!targetVisible()) return {};
    return QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "목표 %1 m")).arg(lengthNumber(service_->target().altitudeRelM));
}
bool FlyController::loiterVisible() const { return service_ && service_->loiter().show; }
double FlyController::loiterRadius() const { return service_ ? service_->loiter().radiusM : 0.0; }
QString FlyController::loiterText() const
{
    if (!loiterVisible()) return {};
    return QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "반지름 %1 m")).arg(qRound(service_->loiter().radiusM));
}
bool FlyController::arm(int sysid)
{
    if (!isActive(sysid)) return false;
    service_->arm();
    return true;
}
bool FlyController::disarm(int sysid)
{
    if (!isActive(sysid)) return false;
    service_->disarm();
    return true;
}
bool FlyController::emergencyStop(int sysid)
{
    if (!isActive(sysid)) return false;
    service_->emergencyStop();
    return true;
}
bool FlyController::land(int sysid)
{
    if (!isActive(sysid)) return false;
    service_->land();
    return true;
}
bool FlyController::rtl(int sysid)
{
    if (!isActive(sysid)) return false;
    service_->rtl();
    return true;
}
bool FlyController::pause(int sysid)
{
    if (!isActive(sysid)) return false;
    service_->pause();
    return true;
}
bool FlyController::takeoff(int sysid, double height)
{
    if (!isActive(sysid)) return false;
    // 성공 여부와 상관없이 보낸 높이를 다음에 여는 창에 보여요.
    if (service_->capabilities().takeoff == TakeoffKind::WithAltitude) setTakeoffAltitude(height);
    service_->takeoff(height);
    return true;
}
bool FlyController::setMode(int sysid, double customMode)
{
    if (!isActive(sysid) || !(customMode >= 0)) return false;
    service_->setMode(static_cast<std::uint32_t>(customMode));
    return true;
}
bool FlyController::goTo(int sysid, double lat, double lon, double altitude)
{
    if (!isActive(sysid)) return false;
    const auto result = service_->goTo(lat, lon, altitude);
    if (result) return true;
    showFailure(FlyActionKind::GoTo, result, QString{});
    // 검사 실패는 기체에 보낸 동작이 아니라 늦은 이유가 따라오지 않아요.
    appendableKind_.reset();
    return false;
}
QString FlyController::goToProblem(double lat, double lon, double altitude) const
{
    if (!service_) return {};
    const auto check = service_->checkGoTo(lat, lon, altitude);
    if (check || check.error() == make(FlyError::NotReady)) return {};
    if (check.error() == make(FlyError::TooFar))
        return QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "기체에서 %1 m 이내만 이동 가능"))
            .arg(lengthNumber(kgc::vehicle::safety::guidedLimits().maxDistanceM));
    if (check.error() == make(FlyError::AltitudeOutOfRange))
        return QCoreApplication::translate("Fly", QT_TRANSLATE_NOOP("Fly", "고도 %1~%2 m 입력"))
            .arg(QString::number(guidedAltitudeFrom()), QString::number(guidedAltitudeTo()));
    return errorText(check.error());
}
QString FlyController::distanceText(double lat, double lon) const
{
    return kgc::fly::distanceText(service_ ? service_->distanceToM(lat, lon) : std::nan(""));
}
QString FlyController::positionText(double lat, double lon) const { return kgc::fly::positionText(lat, lon); }
double FlyController::defaultGoToAltitude() const
{
    const double from = guidedAltitudeFrom();
    const double altitude = service_ ? service_->vehicle().states()->position().altRelM : std::nan("");
    if (!std::isfinite(altitude)) return from;
    return std::clamp(std::round(altitude * 10) / 10, from, guidedAltitudeTo());
}
}
