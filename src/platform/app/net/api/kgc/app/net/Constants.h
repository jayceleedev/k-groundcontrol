// [0.2.0 T3] specs/features/0.2.0-fcflash.md
#pragma once
#include <chrono>
namespace kgc::app::net {
inline constexpr std::chrono::milliseconds kHttpDefaultTimeout{30000};
inline constexpr int kHttpDefaultRetries = 2;
}
