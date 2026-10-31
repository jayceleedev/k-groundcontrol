// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "AttitudeUpdater.h"
#include <QtMath>
#include <cmath>
namespace kgc::vehicle::states {
AttitudeBundle updateAttitude(const mavlink_message_t& message)
{
    mavlink_attitude_t attitude{};
    mavlink_msg_attitude_decode(&message, &attitude);
    auto yaw = std::fmod(qRadiansToDegrees(double(attitude.yaw)), 360.0);
    if (yaw < 0) yaw += 360.0;
    return {qRadiansToDegrees(double(attitude.roll)), qRadiansToDegrees(double(attitude.pitch)), yaw};
}
}
