// [0.6.0 5.11.4] specs/features/0.6.0-fly.md
#pragma once
#include <cstdint>
namespace kgc::fly {
inline constexpr auto kCategory = "kgc.fly";
enum class FlyEvent : std::uint16_t {
    ActionStarted = 1, ActionFinished, ActionFailed, GoToSent, GoToConfirmed, GoToRejected, GoToUnreported, TargetCleared
};
}
