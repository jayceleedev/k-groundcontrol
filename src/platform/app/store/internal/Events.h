// [0.5.0 S1] specs/features/0.5.0-map.md
#pragma once
#include <cstdint>
namespace kgc::app::store {
inline constexpr auto kCategory = "kgc.app.store";
enum class StoreEvent : std::uint16_t { Opened = 1, OpenFailed, QueryFailed };
}
