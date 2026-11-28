// [0.6.0 5.11.3 T9] specs/features/0.6.0-fly.md
#include "FlyChecks.h"
#include "Constants.h"
namespace kgc::fly {
FlyReadiness readinessOf(const FlyInputs& in)
{
    FlyReadiness ready;
    if (!in.linkOk) return ready;
    const auto& caps = in.capabilities;
    const bool airborne = in.landedState == 2 || in.landedState == 3 || in.landedState == 4;
    const bool onGround = in.landedState == 1;
    const bool unknown = !airborne && !onGround;
    const bool known = caps.classKnown;
    const bool flying = airborne || unknown;
    // 이륙 모드에서는 시동을 걸면 곧바로 떠오르므로 이륙 단추로만 시동해요(§2).
    const bool launchMode = in.modeId >= 0 && caps.launchOnArmModes.contains(static_cast<std::uint32_t>(in.modeId));
    ready.arm = !in.armed && !in.busy && !launchMode;
    ready.disarm = in.armed && !airborne;
    ready.emergencyStop = in.armed && flying;
    ready.takeoff = known && caps.takeoff != kgc::vehicle::firmware::TakeoffKind::None && !in.busy
        && (onGround || (unknown && !in.armed));
    ready.land = known && caps.land && in.armed && !in.busy && flying;
    ready.rtl = known && caps.rtl && in.armed && !in.busy && flying;
    ready.pause = known && caps.pause && in.armed && !in.busy && flying;
    ready.setMode = known && !caps.modes.isEmpty() && !in.busy;
    ready.goTo = known && caps.goTo && in.armed && in.landedState == 2 && in.hasPosition && in.hasHome && !in.busy;
    return ready;
}
bool modeChoosable(const kgc::vehicle::firmware::FlightMode& mode, int landedState)
{
    return !mode.pilotControlled || landedState == 1;
}
bool isReason(const kgc::vehicle::states::VehicleMessage& message)
{
    return message.severity <= kReasonSeverityMax && !message.text.isEmpty();
}
}
