// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "GpsUpdater.h"
#include "Constants.h"
namespace kgc::vehicle::states {
GpsBundle updateGps(const mavlink_message_t& message)
{
    mavlink_gps_raw_int_t gps{};
    mavlink_msg_gps_raw_int_decode(&message, &gps);
    GpsBundle result;
    result.fixType = gps.fix_type;
    if (gps.satellites_visible != kUnknownSatellites) result.satellites = gps.satellites_visible;
    if (gps.eph != kUnknownUnsigned16) result.hdop = gps.eph / kCentimetresPerMetre;
    return result;
}
}
