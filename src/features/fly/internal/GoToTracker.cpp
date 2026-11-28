// [0.6.0 5.11.6] specs/features/0.6.0-fly.md
#include "GoToTracker.h"
#include "Constants.h"
#include "Events.h"
#include "FlyChecks.h"
#include <kgc/app/geo/Geodesy.h>
#include <kgc/app/log/Log.h>
#include <kgc/app/scheduler/Timeout.h>
#include <kgc/fly/ModuleId.h>
#include <cmath>
#include <limits>
namespace kgc::fly {
using kgc::app::geo::distanceMeters;
using kgc::vehicle::states::VehicleStates;
GoToTracker::GoToTracker(VehicleStates& states, QObject* parent)
    : QObject(parent), states_(states),
      timer_(new kgc::app::scheduler::Timeout(QStringLiteral("flyTargetConfirm"), this)),
      lastVehicleTarget_{std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()}
{
    connect(&states_, &VehicleStates::navTargetChanged, this, &GoToTracker::receiveTarget);
    connect(&states_, &VehicleStates::armingChanged, this, &GoToTracker::receiveArming);
    connect(&states_, &VehicleStates::flightChanged, this, [this] {
        const bool settled = target_.state == GoToState::Accepted || target_.state == GoToState::Confirmed;
        if (settled && states_.flight().landedState == 1) clear(QStringLiteral("Landed"));
    });
    connect(&states_, &VehicleStates::vehicleMessage, this, [this](const kgc::vehicle::states::VehicleMessage& message) {
        if (isReason(message)) reason_ = message.text;
    });
    connect(timer_, &kgc::app::scheduler::Timeout::expired, this, &GoToTracker::expire);
}
GoToTarget GoToTracker::target() const { return target_; }
void GoToTracker::begin(double latDeg, double lonDeg, double altitudeRelM, std::uint32_t guidedMode)
{
    if (watching_) { watching_ = false; emit watchEnded(false); }
    // 받아들여진 앞 목표는 새 목표가 거부되면 되살릴 수 있게 기억해요(PX4 울타리 밖 목표).
    if (target_.state == GoToState::Accepted || target_.state == GoToState::Confirmed) previous_ = std::pair{target_, guidedMode_};
    timer_->stop();
    mismatches_ = 0;
    seenCount_ = states_.navTarget().targetCount;
    lastVehicleTarget_ = {std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()};
    farSeen_ = false;
    reason_.clear();
    target_ = {latDeg, lonDeg, altitudeRelM, GoToState::Sending};
    guidedMode_ = guidedMode;
    emit changed();
}
void GoToTracker::accept()
{
    target_.state = GoToState::Accepted;
    mismatches_ = 0;
    farSeen_ = false;
    seenCount_ = states_.navTarget().targetCount;
    timer_->start(kTargetConfirmTimeout);
    emit changed();
}
void GoToTracker::watch()
{
    watching_ = true;
    seenCount_ = states_.navTarget().targetCount;
    timer_->start(kTargetConfirmTimeout);
}
void GoToTracker::fail()
{
    const auto arming = states_.arming();
    if (previous_ && arming.armed && arming.modeId == static_cast<qint64>(previous_->second)) restore(previous_->first.state);
    else clear(QStringLiteral("Failed"));
}
void GoToTracker::clear(const QString& cause)
{
    if (watching_) { watching_ = false; emit watchEnded(false); }
    if (target_.state == GoToState::None) return;
    target_ = {};
    previous_.reset();
    timer_->stop();
    KGC_LOG_INFO(kCategory, FlyEvent::TargetCleared, "Go-to target cleared: %1", cause);
    emit changed();
}
void GoToTracker::receiveTarget()
{
    // 새 기체 목표 메시지만 봐요. 선회·만료로 난 알림은 판정과 불일치 셈에 넣지 않아요.
    const auto nav = states_.navTarget();
    if (nav.targetCount == seenCount_) return;
    seenCount_ = nav.targetCount;
    if (!std::isfinite(nav.latDeg) || !std::isfinite(nav.lonDeg)) return;
    lastVehicleTarget_ = {nav.latDeg, nav.lonDeg};
    const double distance = distanceMeters(target_.latDeg, target_.lonDeg, nav.latDeg, nav.lonDeg);
    const bool near = distance <= kTargetMatchMeters;
    if (target_.state == GoToState::Accepted) {
        if (near) {
            target_.state = GoToState::Confirmed;
            timer_->stop();
            KGC_LOG_INFO(kCategory, FlyEvent::GoToConfirmed, "Go-to confirmed: %1 m from target", QString::number(distance, 'f', 1));
            emit changed();
        } else {
            farSeen_ = true;
            farDistanceM_ = distance;
        }
    } else if (target_.state == GoToState::Confirmed) {
        mismatches_ = near ? 0 : mismatches_ + 1;
        if (mismatches_ >= kTargetMismatchCount) clear(QStringLiteral("Replaced"));
    } else if (target_.state == GoToState::Sending && watching_ && near) {
        // 모드는 그대로지만 기체가 목표를 받았어요. 기체가 머문 모드를 떠나면 지워요.
        watching_ = false;
        target_.state = GoToState::Confirmed;
        guidedMode_ = static_cast<std::uint32_t>(states_.arming().modeId);
        previous_.reset();
        timer_->stop();
        KGC_LOG_INFO(kCategory, FlyEvent::GoToConfirmed, "Go-to confirmed: %1 m from target", QString::number(distance, 'f', 1));
        emit changed();
        emit watchEnded(true);
    }
}
void GoToTracker::receiveArming()
{
    if (target_.state != GoToState::Accepted && target_.state != GoToState::Confirmed) return;
    const auto arming = states_.arming();
    if (arming.modeId != static_cast<qint64>(guidedMode_)) clear(QStringLiteral("ModeChanged"));
    else if (!arming.armed) clear(QStringLiteral("Disarmed"));
}
void GoToTracker::expire()
{
    if (target_.state == GoToState::Accepted) {
        if (!farSeen_) {
            KGC_LOG_INFO(kCategory, FlyEvent::GoToUnreported, "Go-to target not reported");
            return;
        }
        KGC_LOG_WARN(kCategory, FlyEvent::GoToRejected, "Go-to rejected: vehicle target %1 m away", QString::number(farDistanceM_, 'f', 1));
        emit rejected(reason_);
        // 기체가 앞 목표로 계속 가면(PX4 울타리 밖 목표를 버림) 앞 목표를 되살려요.
        const bool previousKept = previous_ && distanceMeters(previous_->first.latDeg, previous_->first.lonDeg,
            lastVehicleTarget_.first, lastVehicleTarget_.second) <= kTargetMatchMeters;
        if (previousKept) restore(GoToState::Confirmed);
        else clear(QStringLiteral("Rejected"));
        return;
    }
    if (target_.state == GoToState::Sending && watching_) {
        watching_ = false;
        emit watchEnded(false);
        fail();
    }
}
void GoToTracker::restore(GoToState state)
{
    target_ = previous_->first;
    target_.state = state;
    guidedMode_ = previous_->second;
    previous_.reset();
    timer_->stop();
    emit changed();
}
}
