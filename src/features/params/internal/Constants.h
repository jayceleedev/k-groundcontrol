// [0.3.0 6.2 6.6] specs/features/0.3.0-params.md
#pragma once
#include <chrono>
namespace kgc::params {
inline constexpr std::chrono::milliseconds kParamGapWait{1000};
inline constexpr std::chrono::milliseconds kParamListTimeout{5000};
inline constexpr std::chrono::milliseconds kParamSetTimeout{1000};
inline constexpr std::chrono::milliseconds kParamRegroupDelay{200};
inline constexpr std::chrono::milliseconds kRebootReconnectWait{30000};
inline constexpr std::chrono::milliseconds kRebootOpenDelay{6000};
inline constexpr std::chrono::milliseconds kRebootRetry{1000};
inline constexpr int kParamListRetries = 4;
inline constexpr int kParamReadTries = 5;
inline constexpr int kParamReadBatch = 10;
inline constexpr int kParamSetRetries = 2;
inline constexpr int kParamIndexNone = 65535;
inline constexpr int kParamIdLength = 16;
inline constexpr int kToastNames = 3;
inline constexpr int kParamFloatUlps = 2;
// 고정 소수점 표기는 double의 가장 큰 값과 가장 작은 값도 담아야 해서 길게 잡아요.
inline constexpr int kValueTextSize = 352;
}
