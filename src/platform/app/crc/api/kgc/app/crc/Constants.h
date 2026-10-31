// [0.2.0 T1] specs/features/0.2.0-fcflash.md
#pragma once
#include <cstdint>
namespace kgc::app::crc {
inline constexpr std::uint32_t kBootloaderCrcPolynomial = 0xEDB88320;
inline constexpr std::uint32_t kBootloaderCrcInitial = 0;
}
