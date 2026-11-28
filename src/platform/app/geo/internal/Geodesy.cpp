// [0.6.0 5.1 T3] specs/features/0.6.0-fly.md
#include <kgc/app/geo/Geodesy.h>
#include "Constants.h"
#include <cmath>
#include <numbers>
namespace kgc::app::geo {
namespace {
double radians(double degrees)
{
    return degrees * std::numbers::pi / 180.0;
}
}
double distanceMeters(double lat1Deg, double lon1Deg, double lat2Deg, double lon2Deg)
{
    if (!std::isfinite(lat1Deg) || !std::isfinite(lon1Deg) || !std::isfinite(lat2Deg) || !std::isfinite(lon2Deg))
        return std::nan("");
    const double halfLat = radians(lat2Deg - lat1Deg) / 2.0;
    const double halfLon = radians(lon2Deg - lon1Deg) / 2.0;
    const double a = std::sin(halfLat) * std::sin(halfLat)
        + std::cos(radians(lat1Deg)) * std::cos(radians(lat2Deg)) * std::sin(halfLon) * std::sin(halfLon);
    return 2.0 * kEarthMeanRadiusM * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
}
bool isKnownPosition(double latDeg, double lonDeg)
{
    return std::isfinite(latDeg) && std::isfinite(lonDeg) && !(latDeg == 0 && lonDeg == 0);
}
}
