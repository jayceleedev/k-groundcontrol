// [0.2.0 T4] specs/features/0.2.0-fcflash.md
#pragma once
#include <cstdint>
namespace kgc::comm::rawport {
inline constexpr auto kCategory = "kgc.comm.rawport";
enum class RawPortEvent : std::uint16_t { PortOpened = 1, PortClosed = 2, PortLost = 3 };
}
