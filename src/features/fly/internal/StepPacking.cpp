// [0.6.0 5.11.3 T9] specs/features/0.6.0-fly.md
#include "StepPacking.h"
namespace kgc::fly {
using kgc::vehicle::command::CommandFailure;
kgc::vehicle::command::CommandLong toCommandLong(const kgc::vehicle::firmware::CommandStep& step)
{
    return {step.command, step.param1, step.param2, step.param3, step.param4, step.param5, step.param6, step.param7, 1};
}
kgc::vehicle::command::CommandInt toCommandInt(const kgc::vehicle::firmware::CommandStep& step)
{
    return {step.command, step.frame, step.param1, step.param2, step.param3, step.param4, step.x, step.y, step.z, 1};
}
kgc::vehicle::outbound::OutboundKind outboundKindOf(const kgc::vehicle::firmware::CommandStep& step)
{
    return step.command == MAV_CMD_COMPONENT_ARM_DISARM ? kgc::vehicle::outbound::OutboundKind::Arm
                                                        : kgc::vehicle::outbound::OutboundKind::Motion;
}
FlyError errorOf(MAV_RESULT result)
{
    switch (result) {
    case MAV_RESULT_DENIED: return FlyError::Denied;
    case MAV_RESULT_UNSUPPORTED: return FlyError::Unsupported;
    case MAV_RESULT_TEMPORARILY_REJECTED: return FlyError::TemporarilyRejected;
    default: return FlyError::Failed;
    }
}
FlyError errorOf(CommandFailure failure)
{
    switch (failure) {
    case CommandFailure::Timeout: return FlyError::NoAck;
    case CommandFailure::Gone: return FlyError::Gone;
    case CommandFailure::NoLink: return FlyError::NoLink;
    case CommandFailure::Preempted: return FlyError::Preempted;
    }
    return FlyError::Failed;
}
}
