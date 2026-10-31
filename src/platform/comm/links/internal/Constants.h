// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <chrono>
namespace kgc::comm::links {
inline constexpr quint16 kUdpPort = 14550;
inline constexpr std::chrono::milliseconds kPortListPeriod{1000};
inline constexpr std::uint8_t kLinkChannel = 0;
}
