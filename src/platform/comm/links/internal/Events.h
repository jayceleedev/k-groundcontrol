// [0.1.1 T4 T5 T13 T15 T16] specs/features/0.1.1-link-hardening.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <cstdint>
namespace kgc::comm::links {
inline constexpr auto kCategory = "kgc.comm.links";
enum class LinksEvent : std::uint16_t { LinkWorkerStopped = 1, SerialErrorIgnored = 2, SerialPortLost = 3, SerialWriteBusy = 4 };
}
