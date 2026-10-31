// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <chrono>
#include <cstdint>
namespace kgc::app::constants {
inline constexpr std::chrono::milliseconds kGcsHeartbeatPeriod{1000};
inline constexpr std::chrono::milliseconds kLinkLossTimeout{3500};
inline constexpr std::uint8_t kGcsSysid = 255;
inline constexpr std::uint8_t kGcsCompid = 190;
}
