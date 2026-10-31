// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "Px4Plugin.h"
#include <array>
namespace kgc::vehicle::firmware {
QString Px4Plugin::displayName() const { return "PX4"; }
QString Px4Plugin::modeName(std::uint32_t customMode, std::uint8_t) const
{
    const auto main = (customMode >> 16) & 0xff;
    const auto sub = (customMode >> 24) & 0xff;
    if (main == 3) {
        constexpr std::array names{"Position", "Orbit", "Position Slow"};
        return sub < names.size() ? QString::fromLatin1(names[sub]) : QString{};
    }
    if (main == 4) {
        constexpr std::array names{"", "Ready", "Takeoff", "Hold", "Mission", "Return", "Land", "",
            "Follow Me", "Precision Land", "VTOL Takeoff", "External 1", "External 2", "External 3",
            "External 4", "External 5", "External 6", "External 7", "External 8"};
        return sub < names.size() ? QString::fromLatin1(names[sub]) : QString{};
    }
    if (sub != 0) return {};
    constexpr std::array names{"", "Manual", "Altitude", "", "", "Acro", "Offboard",
        "Stabilized", "Rattitude", "", "Termination", "Altitude Cruise"};
    return main < names.size() ? QString::fromLatin1(names[main]) : QString{};
}
QList<StreamRequest> Px4Plugin::streamRequests() const { return {}; }
}
