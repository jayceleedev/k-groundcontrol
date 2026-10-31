// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
#pragma once
#include <array>
#include <chrono>
namespace kgc::link {
inline constexpr std::array kBaudRates{57600, 115200, 230400, 460800, 921600};
inline constexpr int kDefaultBaudRate = 115200;
inline constexpr std::chrono::milliseconds kNoVehicleTimeout{10000};
inline constexpr std::chrono::milliseconds kNoVehicleCheckPeriod{1000};
}
