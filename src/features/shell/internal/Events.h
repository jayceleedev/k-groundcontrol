// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#pragma once
#include <cstdint>
namespace kgc::shell {
inline constexpr auto kCategory = "kgc.shell";
enum class ShellEvent : std::uint16_t { AppStarted = 1, WindowShown, PanelOpened, PanelClosed, MenuCollapsed, AppStopped };
}
