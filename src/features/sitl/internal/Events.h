// [0.4.0 6.9] specs/features/0.4.0-sitl.md
#pragma once
#include <cstdint>
namespace kgc::sitl {
inline constexpr auto kCategory = "kgc.sitl";
enum class SitlEvent : std::uint16_t {
    SitlDownloadStarted = 1, SitlDownloadFinished = 2, SitlDownloadFailed = 3, SitlStarted = 4,
    SitlStartFailed = 5, SitlExited = 6, SitlLinkOpened = 7, SitlLinkFailed = 8
};
}
