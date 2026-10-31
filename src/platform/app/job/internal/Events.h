// [0.2.0 T2] specs/features/0.2.0-fcflash.md
#pragma once
#include <cstdint>
namespace kgc::app::job {
inline constexpr auto kCategory = "kgc.app.job";
enum class JobEvent : std::uint16_t { JobFinished = 1 };
}
