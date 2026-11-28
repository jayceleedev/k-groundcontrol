// [0.6.0 5.5] specs/features/0.6.0-fly.md
#pragma once
#include <cstdint>
namespace kgc::vehicle::firmware {
// MAVLink 전역 좌표의 위도·경도 배율(도 × 1e7)이에요.
inline constexpr double kCoordinateScale = 10000000.0;
// ArduPilot에 요청하는 EXTENDED_SYS_STATE 간격(1 Hz, µs)이에요.
inline constexpr std::int32_t kExtendedSysStateIntervalUs = 1000000;
// ArduPlane LOITER_RADIUS_DEFAULT(m)예요. WP_LOITER_RAD 절댓값이 kLoiterRadiusSmallM 이하이면 이 값으로 돌아요.
inline constexpr double kLoiterRadiusDefaultM = 60.0;
inline constexpr double kLoiterRadiusSmallM = 1.0;
// PX4가 COMMAND_INT 위도·경도 INT32_MAX를 "그대로"로 읽는 첫 버전(1.12)이에요.
inline constexpr int kPx4KeepLatLonMajor = 1;
inline constexpr int kPx4KeepLatLonMinor = 12;
}
