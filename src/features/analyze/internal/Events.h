// [0.0.3 T3] specs/features/0.0.3-windows.md
#pragma once
#include <cstdint>
namespace kgc::analyze {
inline constexpr char kCategory[] = "kgc.analyze";
enum class AnalyzeEvent : std::uint16_t { LogWindowOpened = 1 };
}
