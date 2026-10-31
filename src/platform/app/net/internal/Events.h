// [0.2.0 T3] specs/features/0.2.0-fcflash.md
#pragma once
#include <cstdint>
namespace kgc::app::net {
inline constexpr auto kCategory = "kgc.app.net";
enum class NetEvent : std::uint16_t { RequestFailed = 1, RedirectRefused = 2 };
}
