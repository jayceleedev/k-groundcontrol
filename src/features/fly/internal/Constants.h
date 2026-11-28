// [0.6.0 §12 2] specs/features/0.6.0-fly.md
// [0.6.0 5.11.4] specs/features/0.6.0-fly.md
#pragma once
#include <chrono>
namespace kgc::fly {
// ACK 뒤 HEARTBEAT로 모드·시동 상태를 확인하는 시간이에요(HEARTBEAT 1 Hz 세 번).
inline constexpr std::chrono::milliseconds kConfirmTimeout{3000};
// 여기로 가기가 받아들여진 뒤 기체 목표(POSITION_TARGET_GLOBAL_INT)를 기다리는 시간이에요.
inline constexpr std::chrono::milliseconds kTargetConfirmTimeout{5000};
// 기체 목표가 보낸 곳에서 이 거리 안이면 같은 목표로 봐요.
inline constexpr double kTargetMatchMeters = 10.0;
// 확인된 목표에서 먼 기체 목표가 이만큼 잇달아 오면 다른 목표로 바뀐 것으로 봐요.
inline constexpr int kTargetMismatchCount = 2;
// 이번 실행에서 처음 여는 이륙 창의 높이예요(지금 자리 위, m).
inline constexpr double kDefaultTakeoffAltitudeM = 10.0;
// 실패 까닭으로 덧붙이는 기체 메시지의 가장 낮은 중요도예요(MAV_SEVERITY_NOTICE).
inline constexpr int kReasonSeverityMax = 5;
// 이유 없이 실패한 뒤 늦게 오는 기체 이유 글을 더 기다리는 시간이에요(Timeout flyReasonGrace, §12 고침 2).
inline constexpr std::chrono::milliseconds kReasonGrace{1000};
}
