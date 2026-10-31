// [0.2.0 T6] specs/features/0.2.0-fcflash.md
#pragma once
#include <cstdint>
namespace kgc::vehicle::command {
inline constexpr auto kCategory = "kgc.vehicle.command";
enum class CommandEvent : std::uint16_t { CommandRetried = 1, CommandTimedOut = 2 };
}
