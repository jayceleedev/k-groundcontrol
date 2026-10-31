// [0.2.0 T11] specs/features/0.2.0-fcflash.md
#include "AutopilotVersionUpdater.h"
namespace kgc::vehicle::states {
CapabilitiesBundle updateAutopilotVersion(const mavlink_message_t& message, CapabilitiesBundle current)
{
    // WORKER-FILL: spec §5.8 — mavlink_msg_autopilot_version_decode 뒤 flight_sw_version을 major.minor.patch로 풀어요.
    Q_UNUSED(message);
    return current;
}
}
