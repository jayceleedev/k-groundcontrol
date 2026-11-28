// [0.6.0 §12 3] specs/features/0.6.0-fly.md
// [0.6.0 5.4] specs/features/0.6.0-fly.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <chrono>
namespace kgc::vehicle::states {
inline constexpr double kCoordinateScale = 10000000.0;
inline constexpr double kCentimetresPerMetre = 100.0;
inline constexpr double kMilliUnits = 1000.0;
inline constexpr unsigned kUnknownUnsigned16 = 65535;
inline constexpr unsigned kUnknownSatellites = 255;
// 항법 목표는 ArduPilot 2 Hz·PX4 1 Hz보다 넉넉히 3초 안 오면 비워요.
inline constexpr std::chrono::milliseconds kNavTargetStale{3000};
// 비행 상태는 EXTENDED_SYS_STATE 1 Hz 다섯 번, 5초 안 오면 비워요.
inline constexpr std::chrono::milliseconds kFlightStale{5000};
// 위치를 앎으로 보는 가장 낮은 GPS fix_type이에요(3 = 3D). 그 전에는 홈을 알 때만 위치를 채워요.
inline constexpr int kPositionFixMin = 3;
}
