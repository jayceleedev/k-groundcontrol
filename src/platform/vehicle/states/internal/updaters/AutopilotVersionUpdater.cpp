// [0.2.0 T11] specs/features/0.2.0-fcflash.md
#include "AutopilotVersionUpdater.h"
namespace kgc::vehicle::states {
CapabilitiesBundle updateAutopilotVersion(const mavlink_message_t& message, CapabilitiesBundle current)
{
    mavlink_autopilot_version_t version{};
    mavlink_msg_autopilot_version_decode(&message, &version);
    const auto value = version.flight_sw_version;
    current.firmwareVersion = value == 0 ? QString() : QStringLiteral("%1.%2.%3")
        .arg(value >> 24).arg((value >> 16) & 0xFF).arg((value >> 8) & 0xFF);
    return current;
}
}
