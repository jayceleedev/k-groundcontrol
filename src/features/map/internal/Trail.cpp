// [0.6.0 5.1] specs/features/0.6.0-fly.md
// [0.5.0 5.6.3 T4] specs/features/0.5.0-map.md
#include "Trail.h"
#include <kgc/app/geo/Geodesy.h>
namespace kgc::map {
bool isMapPosition(double latDeg, double lonDeg)
{
    return kgc::app::geo::isKnownPosition(latDeg, lonDeg);
}
bool appendTrailPoint(QList<QGeoCoordinate>& points, const QGeoCoordinate& next, double minMeters, int maxPoints)
{
    if (!isMapPosition(next.latitude(), next.longitude())) return false;
    if (!points.isEmpty() && points.constLast().distanceTo(next) <= minMeters) return false;
    points.append(next);
    if (points.size() > maxPoints) points.remove(0, points.size() - maxPoints);
    return true;
}
}
