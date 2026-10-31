// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "ArmingUpdater.h"
namespace kgc::vehicle::states {
ArmingBundle updateArming(const mavlink_message_t& message, const kgc::vehicle::firmware::FirmwarePlugin& plugin)
{
    mavlink_heartbeat_t heartbeat{};
    mavlink_msg_heartbeat_decode(&message, &heartbeat);
    return {bool(heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED), heartbeat.custom_mode,
            plugin.modeName(heartbeat.custom_mode, heartbeat.type)};
}
}
