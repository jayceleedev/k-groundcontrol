// [0.1.1 T10] specs/features/0.1.1-link-hardening.md
#pragma once
#include <chrono>
namespace kgc::vehicle::outbound {
inline constexpr std::chrono::milliseconds kStreamRequestPeriod{500};
inline constexpr int kStreamRequestRounds = 5;
}
