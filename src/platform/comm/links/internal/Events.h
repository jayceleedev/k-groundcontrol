// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <cstdint>
namespace kgc::comm::links {
inline constexpr auto kCategory = "kgc.comm.links";
enum class LinksEvent : std::uint16_t { LinkWorkerStopped = 1 };
}
