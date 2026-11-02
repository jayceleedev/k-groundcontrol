// [0.3.0 6.9] specs/features/0.3.0-params.md
#pragma once
#include <cstdint>
namespace kgc::params {
inline constexpr auto kCategory = "kgc.params";
enum class ParamEvent : std::uint16_t {
    DownloadStarted = 1, DownloadFinished = 2, DownloadFailed = 3, WriteFinished = 4, SetFailed = 5,
    BackupWritten = 6, MetadataLoaded = 7, MetadataFailed = 8, RebootSent = 9, RebootReconnect = 10
};
}
