// [0.6.0 5.4 T2] specs/features/0.6.0-fly.md
#include "NavTargetUpdater.h"
#include "Constants.h"
#include <limits>
namespace kgc::vehicle::states {
namespace {
constexpr double kUnknown = std::numeric_limits<double>::quiet_NaN();
bool isGlobalFrame(std::uint8_t frame)
{
    switch (frame) {
    case MAV_FRAME_GLOBAL:
    case MAV_FRAME_GLOBAL_INT:
    case MAV_FRAME_GLOBAL_RELATIVE_ALT:
    case MAV_FRAME_GLOBAL_RELATIVE_ALT_INT:
    case MAV_FRAME_GLOBAL_TERRAIN_ALT:
    case MAV_FRAME_GLOBAL_TERRAIN_ALT_INT:
        return true;
    default:
        return false;
    }
}
}
NavTargetBundle updateNavTarget(const mavlink_message_t& message, NavTargetBundle current)
{
    mavlink_position_target_global_int_t target{};
    mavlink_msg_position_target_global_int_decode(&message, &target);
    const bool ignored = (target.type_mask & (POSITION_TARGET_TYPEMASK_X_IGNORE | POSITION_TARGET_TYPEMASK_Y_IGNORE)) != 0;
    const bool zero = target.lat_int == 0 && target.lon_int == 0;
    current.latDeg = ignored || zero ? kUnknown : target.lat_int / kCoordinateScale;
    current.lonDeg = ignored || zero ? kUnknown : target.lon_int / kCoordinateScale;
    current.altM = target.alt;
    ++current.targetCount;
    return current;
}
NavTargetBundle updateOrbit(const mavlink_message_t& message, NavTargetBundle current)
{
    mavlink_orbit_execution_status_t orbit{};
    mavlink_msg_orbit_execution_status_decode(&message, &orbit);
    current.orbitRadiusM = orbit.radius == 0 ? kUnknown : static_cast<double>(orbit.radius);
    // PX4는 frame 0(MAV_FRAME_GLOBAL)을 보내요(FixedwingPositionControl.cpp v1.16.2:3155).
    const bool global = isGlobalFrame(orbit.frame);
    current.orbitLatDeg = global ? orbit.x / kCoordinateScale : kUnknown;
    current.orbitLonDeg = global ? orbit.y / kCoordinateScale : kUnknown;
    return current;
}
bool isStale(qint64 receivedMs, qint64 nowMs, std::chrono::milliseconds limit)
{
    return nowMs - receivedMs > limit.count();
}
}
