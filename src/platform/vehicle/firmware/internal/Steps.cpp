// [0.6.0 5.5 T6] specs/features/0.6.0-fly.md
#include "Steps.h"
#include <kgc/comm/mavlink/Mavlink.h>
namespace kgc::vehicle::firmware {
CommandStep armStep(bool arm, float force)
{
    CommandStep step;
    step.command = MAV_CMD_COMPONENT_ARM_DISARM;
    step.param1 = arm ? 1.0F : 0.0F;
    step.param2 = force;
    step.confirmArmed = arm;
    return step;
}
CommandStep modeStep(float param2, float param3, QList<std::uint32_t> confirm)
{
    CommandStep step;
    step.command = MAV_CMD_DO_SET_MODE;
    step.param1 = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
    step.param2 = param2;
    step.param3 = param3;
    step.confirmModes = std::move(confirm);
    return step;
}
CommandStep repositionStep(std::uint8_t frame, float param4, std::int32_t x, std::int32_t y, float z, std::uint32_t confirm)
{
    CommandStep step;
    step.form = CommandForm::Int;
    step.command = MAV_CMD_DO_REPOSITION;
    step.param1 = -1.0F;
    step.param2 = MAV_DO_REPOSITION_FLAGS_CHANGE_MODE;
    step.param3 = 0.0F;
    step.param4 = param4;
    step.frame = frame;
    step.x = x;
    step.y = y;
    step.z = z;
    step.confirmModes = {confirm};
    return step;
}
std::uint32_t px4Mode(std::uint32_t main, std::uint32_t sub)
{
    return (main << 16) | (sub << 24);
}
}
