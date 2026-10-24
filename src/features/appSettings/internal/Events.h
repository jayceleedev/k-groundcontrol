// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#pragma once
#include <cstdint>
namespace kgc::appSettings {
inline constexpr auto kCategory = "kgc.appSettings";
enum class AppSettingsEvent : std::uint16_t { LanguageChanged = 1 };
}
