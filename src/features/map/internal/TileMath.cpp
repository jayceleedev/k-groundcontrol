// [0.5.0 5.6.3 T2] specs/features/0.5.0-map.md
#include "TileMath.h"
#include "Constants.h"
#include <QtMath>
#include <algorithm>
#include <cmath>
#include <numbers>
namespace kgc::map {
TileXY tileOf(double lat, double lon, int z)
{
    const double tiles = double(qint64(1) << z);
    const double phi = qDegreesToRadians(std::clamp(lat, -kMaxMercatorLatitude, kMaxMercatorLatitude));
    const double x = std::floor((lon + 180) / 360 * tiles);
    const double y = std::floor((1 - std::log(std::tan(phi) + 1 / std::cos(phi)) / std::numbers::pi) / 2 * tiles);
    // 끝의 위도·경도(±85.05…, 180)는 계산상 한 칸 밖으로 나가므로 0 ~ 2^z − 1로 잘라요. NaN도 0으로 둬요.
    const auto inside = [tiles](double value) { return value >= 0 ? int(std::min(value, tiles - 1)) : 0; };
    return {inside(x), inside(y)};
}
TileRange tileRange(const QGeoRectangle& area, int z)
{
    const auto topLeft = tileOf(area.topLeft().latitude(), area.topLeft().longitude(), z);
    const auto bottomRight = tileOf(area.bottomRight().latitude(), area.bottomRight().longitude(), z);
    const int world = 1 << z;
    const int rows = bottomRight.y - topLeft.y + 1;
    // 왼쪽 위 경도가 더 크면 날짜 변경선을 넘어 오른쪽 끝에서 왼쪽 끝으로 이어지는 영역이에요.
    const bool wraps = area.topLeft().longitude() > area.bottomRight().longitude();
    const int columns = wraps ? std::min(world, bottomRight.x - topLeft.x + world + 1) : bottomRight.x - topLeft.x + 1;
    return {topLeft.x, topLeft.y, columns, rows};
}
qint64 tileCount(const QGeoRectangle& area, int minZoom, int maxZoom)
{
    qint64 count = 0;
    for (int z = minZoom; z <= maxZoom; ++z) {
        const auto range = tileRange(area, z);
        count += qint64(range.columns) * range.rows;
    }
    return count;
}
double metersPerPixel(double lat, double zoom)
{
    return kMetersPerPixelAtZoom0 * std::cos(qDegreesToRadians(lat)) / std::exp2(zoom);
}
}
