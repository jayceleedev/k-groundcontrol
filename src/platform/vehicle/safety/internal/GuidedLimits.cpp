// [0.6.0 5.7 T8] specs/features/0.6.0-fly.md
#include <kgc/vehicle/safety/GuidedLimits.h>
#include <kgc/safety/Settings.h>
#include <utility>
namespace kgc::vehicle::safety {
GuidedLimits guidedLimits()
{
    GuidedLimits limits{kgc::safety::settings::guidedMaxDistanceM(), kgc::safety::settings::guidedMinAltitudeM(),
                        kgc::safety::settings::guidedMaxAltitudeM()};
    // 사용자가 최저를 최고보다 크게 적어도 한계가 비지 않게 둘을 바꿔요.
    if (limits.minAltitudeM > limits.maxAltitudeM) std::swap(limits.minAltitudeM, limits.maxAltitudeM);
    return limits;
}
GuidedCheck checkGuided(const GuidedLimits& limits, double distanceM, double altitudeRelM)
{
    // 부정 비교라 NaN도 한계 밖으로 걸러져요.
    if (!(distanceM <= limits.maxDistanceM)) return GuidedCheck::TooFar;
    if (!(altitudeRelM >= limits.minAltitudeM)) return GuidedCheck::TooLow;
    if (altitudeRelM > limits.maxAltitudeM) return GuidedCheck::TooHigh;
    return GuidedCheck::Ok;
}
}
