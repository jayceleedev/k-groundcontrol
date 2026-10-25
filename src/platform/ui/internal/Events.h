// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
#pragma once
#include <cstdint>
namespace kgc::ui {
inline constexpr auto kCategory = "kgc.ui";
enum class UiEvent : std::uint16_t { ThemeApplied = 1, ThemeUnknown, BackgroundImageMissing };
}
