// [0.5.0 S3 T5] specs/features/0.5.0-map.md
#include "HomePositionUpdater.h"
#include "Constants.h"
namespace kgc::vehicle::states {
PositionBundle updateHomePosition(const mavlink_message_t& message, PositionBundle current)
{
    mavlink_home_position_t home{};
    mavlink_msg_home_position_decode(&message, &home);
    current.homeLatDeg = home.latitude / kCoordinateScale;
    current.homeLonDeg = home.longitude / kCoordinateScale;
    current.homeAltAmslM = home.altitude / kMilliUnits;
    return current;
}
}
