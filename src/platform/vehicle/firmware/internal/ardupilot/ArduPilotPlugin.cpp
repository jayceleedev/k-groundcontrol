// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "ArduPilotPlugin.h"
#include <kgc/comm/mavlink/Mavlink.h>
#include <array>
namespace kgc::vehicle::firmware {
QString ArduPilotPlugin::displayName() const { return "ArduPilot"; }
QString ArduPilotPlugin::modeName(std::uint32_t customMode, std::uint8_t mavType) const
{
    switch (mavType) {
    case MAV_TYPE_QUADROTOR: case MAV_TYPE_HEXAROTOR: case MAV_TYPE_OCTOROTOR:
    case MAV_TYPE_TRICOPTER: case MAV_TYPE_HELICOPTER: case MAV_TYPE_COAXIAL:
    case MAV_TYPE_DODECAROTOR: case MAV_TYPE_DECAROTOR: break;
    default: return {};
    }
    constexpr std::array names{"STABILIZE", "ACRO", "ALT_HOLD", "AUTO", "GUIDED", "LOITER", "RTL",
        "CIRCLE", "", "LAND", "", "DRIFT", "", "SPORT", "FLIP", "AUTOTUNE", "POSHOLD", "BRAKE",
        "THROW", "AVOID_ADSB", "GUIDED_NOGPS", "SMART_RTL", "FLOWHOLD", "FOLLOW", "ZIGZAG",
        "SYSTEMID", "AUTOROTATE", "AUTO_RTL", "TURTLE"};
    return customMode < names.size() ? QString::fromLatin1(names[customMode]) : QString{};
}
QList<StreamRequest> ArduPilotPlugin::streamRequests() const
{
    return {{MAV_DATA_STREAM_EXTENDED_STATUS, 2}, {MAV_DATA_STREAM_POSITION, 4}, {MAV_DATA_STREAM_EXTRA1, 10}};
}
}
