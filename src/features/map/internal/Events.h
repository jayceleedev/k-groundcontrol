// [0.5.1 5.10 9] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.4 6] specs/features/0.5.0-map.md
#pragma once
#include <cstdint>
namespace kgc::map {
inline constexpr auto kCategory = "kgc.map";
enum class MapEvent : std::uint16_t {
    StoreReset = 1, StoreFailed, TileFailed, DownloadStarted, DownloadFinished, DownloadCancelled, LayerFailed,
    TileRatio, WheelInput
};
}
