// [0.6.0 5.11.5] specs/features/0.6.0-fly.md
#include "FlyRunner.h"
#include "Constants.h"
#include "FlyChecks.h"
#include "StepPacking.h"
#include <kgc/app/scheduler/Timeout.h>
#include <kgc/vehicle/command/CommandQueue.h>
#include <kgc/vehicle/states/VehicleStates.h>
namespace kgc::fly {
namespace {
using kgc::vehicle::command::CommandFailure;
using kgc::vehicle::command::CommandResult;
using kgc::vehicle::firmware::CommandStep;
// 확인할 모드가 없거나, 지금 모드가 그 목록에 있으면 참이에요.
bool modeMatches(const CommandStep& step, qint64 modeId)
{
    return step.confirmModes.isEmpty() || (modeId >= 0 && step.confirmModes.contains(static_cast<std::uint32_t>(modeId)));
}
}
FlyRunner::FlyRunner(kgc::vehicle::registry::Vehicle& vehicle, QObject* parent)
    : QObject(parent), vehicle_(vehicle), timeout_(new kgc::app::scheduler::Timeout(QStringLiteral("flyConfirm"), this))
{
    using kgc::vehicle::states::VehicleStates;
    connect(vehicle_.states(), &VehicleStates::vehicleMessage, this, [this](const kgc::vehicle::states::VehicleMessage& message) {
        if (active_ && isReason(message)) reason_ = message.text;
    });
    connect(vehicle_.states(), &VehicleStates::armingChanged, this, [this] {
        if (!waiting_ || !confirmed()) return;
        waiting_ = false;
        timeout_->stop();
        next();
    });
    connect(timeout_, &kgc::app::scheduler::Timeout::expired, this, [this] {
        if (!waiting_) return;
        waiting_ = false;
        if (waitError_) { finish(std::unexpected(make(*waitError_))); return; }
        const bool modeOk = modeMatches(steps_[index_], vehicle_.states()->arming().modeId);
        finish(std::unexpected(make(modeOk ? FlyError::ArmNotChanged : FlyError::ModeNotChanged)));
    });
}
bool FlyRunner::busy() const { return active_; }
void FlyRunner::start(FlyActionKind kind, QList<CommandStep> steps, bool preempt)
{
    // 남은 실행은 끼어든 명령에 밀린 것으로 먼저 끝내요(시동 끄기·비상 정지만 이렇게 불러요).
    if (active_) finish(std::unexpected(make(FlyError::Preempted)));
    ++run_;
    active_ = true;
    kind_ = kind;
    steps_ = std::move(steps);
    index_ = 0;
    preempt_ = preempt;
    reason_.clear();
    if (steps_.isEmpty()) { finish({}); return; }
    sendStep();
}
void FlyRunner::sendStep()
{
    const auto& step = steps_[index_];
    kgc::vehicle::command::CommandOptions options;
    options.kind = outboundKindOf(step);
    options.preempt = preempt_ && index_ == 0;
    // 끝난 실행의 늦은 응답은 실행 번호가 달라 버려요.
    auto done = [this, run = run_](CommandResult result) { if (run == run_ && active_) receive(result); };
    // 문맥은 늘 이 실행기예요. 끼어들기는 같은 문맥으로 줄 선 명령만 지워요(§5.3).
    if (step.form == kgc::vehicle::firmware::CommandForm::Long)
        vehicle_.commands()->send(toCommandLong(step), options, std::move(done), this);
    else
        vehicle_.commands()->send(toCommandInt(step), options, std::move(done), this);
}
void FlyRunner::receive(const CommandResult& result)
{
    // 시동 끄기·비상 정지 단계는 ACK가 끼어든 명령의 늦은 응답일 수 있어 HEARTBEAT 시동 상태로 정해요.
    const bool disarming = steps_[index_].confirmArmed == false;
    if (result.failure) {
        if (disarming && *result.failure == CommandFailure::Timeout && confirmed()) { next(); return; }
        finish(std::unexpected(make(errorOf(*result.failure))));
        return;
    }
    const auto ack = result.result.value_or(MAV_RESULT_FAILED);
    if (ack != MAV_RESULT_ACCEPTED && !disarming) {
        // 거부된 시동 뒤 조종기로 시동이 걸려도 다음 단계(NAV_TAKEOFF)를 보내지 않게 곧바로 끝내요.
        finish(std::unexpected(make(errorOf(ack))));
        return;
    }
    if (confirmed()) { next(); return; }
    wait(ack == MAV_RESULT_ACCEPTED ? std::nullopt : std::optional<FlyError>(errorOf(ack)));
}
bool FlyRunner::confirmed() const
{
    const auto arming = vehicle_.states()->arming();
    const auto& step = steps_[index_];
    return modeMatches(step, arming.modeId) && (!step.confirmArmed || arming.armed == *step.confirmArmed);
}
void FlyRunner::wait(std::optional<FlyError> error)
{
    waitError_ = error;
    waiting_ = true;
    timeout_->start(kConfirmTimeout);
}
void FlyRunner::next()
{
    ++index_;
    if (index_ >= steps_.size()) finish({});
    else sendStep();
}
void FlyRunner::finish(kgc::app::errors::Result<void> result)
{
    waiting_ = false;
    timeout_->stop();
    active_ = false;
    const bool quiet = result.has_value() || result.error() == make(FlyError::Preempted);
    const QString reason = quiet ? QString{} : reason_;
    steps_.clear();
    reason_.clear();
    emit finished(kind_, result, reason);
}
}
