// [0.2.0 T5] specs/features/0.2.0-fcflash.md
#pragma once
#include <cstdint>
namespace kgc::vehicle::safety {
inline constexpr auto kCategory = "kgc.vehicle.safety";
enum class SafetyEvent : std::uint16_t { OperationDenied = 1 };
}
