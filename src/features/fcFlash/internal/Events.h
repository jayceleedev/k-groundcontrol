// [0.2.0 T14–T22] specs/features/0.2.0-fcflash.md
#pragma once
#include <cstdint>
namespace kgc::fcFlash {
inline constexpr auto kCategory = "kgc.fcFlash";
enum class FcFlashEvent : std::uint16_t {
    StepStarted = 1, BoardIdentified = 2, FileChosen = 3, FlashFinished = 4, ReconnectFinished = 5
};
}
