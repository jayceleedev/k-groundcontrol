// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "PositionUpdater.h"
#include "Constants.h"
#include <cmath>
namespace kgc::vehicle::states {
PositionBundle updatePosition(const mavlink_message_t& message)
{
    mavlink_global_position_int_t position{};
    mavlink_msg_global_position_int_decode(&message, &position);
    PositionBundle result;
    result.latDeg = position.lat / kCoordinateScale;
    result.lonDeg = position.lon / kCoordinateScale;
    result.altAmslM = position.alt / kMilliUnits;
    result.altRelM = position.relative_alt / kMilliUnits;
    result.groundSpeedMs = std::hypot(double(position.vx), double(position.vy)) / kCentimetresPerMetre;
    result.climbMs = -double(position.vz) / kCentimetresPerMetre;
    if (position.hdg != kUnknownUnsigned16) result.headingDeg = position.hdg / kCentimetresPerMetre;
    return result;
}
}
