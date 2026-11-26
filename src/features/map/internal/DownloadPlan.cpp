// [0.5.0 5.6.3 T6] specs/features/0.5.0-map.md
#include "DownloadPlan.h"
#include "Constants.h"
#include "TileMath.h"
#include <kgc/map/Errors.h>
namespace kgc::map {
kgc::app::errors::Result<qint64> planDownload(const QGeoRectangle& area, int minZoom, int maxZoom)
{
    if (!(kMinZoom <= minZoom && minZoom <= maxZoom && maxZoom <= kMaxZoom))
        return std::unexpected(make(MapError::BadZoomRange));
    const qint64 count = tileCount(area, minZoom, maxZoom);
    if (count > kMaxDownloadTiles) return std::unexpected(make(MapError::TooManyTiles));
    return count;
}
}
