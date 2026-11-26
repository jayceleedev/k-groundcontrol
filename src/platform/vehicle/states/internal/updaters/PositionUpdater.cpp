// [0.5.0 S3 T5] specs/features/0.5.0-map.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "PositionUpdater.h"
#include "Constants.h"
#include <cmath>
#include <limits>
namespace kgc::vehicle::states {
PositionBundle updatePosition(const mavlink_message_t& message, PositionBundle current)
{
    mavlink_global_position_int_t position{};
    mavlink_msg_global_position_int_decode(&message, &position);
    current.latDeg = position.lat / kCoordinateScale;
    current.lonDeg = position.lon / kCoordinateScale;
    current.altAmslM = position.alt / kMilliUnits;
    current.altRelM = position.relative_alt / kMilliUnits;
    current.groundSpeedMs = std::hypot(double(position.vx), double(position.vy)) / kCentimetresPerMetre;
    current.climbMs = -double(position.vz) / kCentimetresPerMetre;
    // 모르는 헤딩을 이전 값으로 이어 받으면 멈춘 방향을 사실처럼 보이므로 NaN으로 둬요.
    current.headingDeg = position.hdg != kUnknownUnsigned16 ? position.hdg / kCentimetresPerMetre
                                                            : std::numeric_limits<double>::quiet_NaN();
    return current;
}
}
