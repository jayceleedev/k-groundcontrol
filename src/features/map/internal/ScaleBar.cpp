// [0.5.0 5.6.3 T3] specs/features/0.5.0-map.md
#include "ScaleBar.h"
#include <cmath>
namespace kgc::map {
ScaleBarLength scaleBar(double metersPerPixel, int maxPixels)
{
    const double room = metersPerPixel * maxPixels;
    ScaleBarLength best{1, int(std::lround(1 / metersPerPixel))};
    // 1 m부터 열 배씩 올라가며 1·2·5 단계 가운데 폭 안에 드는 마지막 것을 남겨요.
    for (double decade = 1; decade <= room; decade *= 10) {
        for (const double step : {1.0, 2.0, 5.0}) {
            const double meters = step * decade;
            if (meters <= room) best = {meters, int(std::lround(meters / metersPerPixel))};
        }
    }
    return best;
}
QString scaleLabel(double meters)
{
    if (meters < 1000) return QStringLiteral("%1 m").arg(std::llround(meters));
    const double kilometers = meters / 1000;
    const bool whole = kilometers == std::floor(kilometers);
    return QStringLiteral("%1 km").arg(whole ? QString::number(std::llround(kilometers)) : QString::number(kilometers, 'f', 1));
}
}
