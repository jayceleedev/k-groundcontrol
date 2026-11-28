// [0.6.0 5.5] specs/features/0.6.0-fly.md
// [0.1.1 T1 T2 T3] specs/features/0.1.1-link-hardening.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "ArduPilotPlugin.h"
#include <kgc/comm/mavlink/Mavlink.h>
namespace kgc::vehicle::firmware {
QString ArduPilotPlugin::displayName() const { return "ArduPilot"; }
QList<StreamRequest> ArduPilotPlugin::streamRequests() const
{
    return {{MAV_DATA_STREAM_EXTENDED_STATUS, 2}, {MAV_DATA_STREAM_POSITION, 4}, {MAV_DATA_STREAM_EXTRA1, 10}};
}
}
