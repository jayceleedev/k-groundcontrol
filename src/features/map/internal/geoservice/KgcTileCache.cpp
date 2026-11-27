// [0.5.1 5.5] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.7] specs/features/0.5.0-map.md
#include "KgcTileCache.h"
#include "Constants.h"
namespace kgc::map {
KgcTileCache::KgcTileCache(int pixelRatio, QObject* parent) : QGeoFileTileCache(QString(), parent), pixelRatio_(pixelRatio) {}
void KgcTileCache::init()
{
    // 2배 그림은 장당 바이트가 약 4배라, 같은 장 수를 담도록 배수²배로 잡아요.
    setMaxDiskUsage(0);
    setMaxMemoryUsage(kTileMemoryCacheBytes * pixelRatio_ * pixelRatio_);
    setExtraTextureUsage(kTileExtraTextureBytes * pixelRatio_ * pixelRatio_);
}
void KgcTileCache::clearAll()
{
    textureCache_.clear();
    memoryCache_.clear();
    diskCache_.clear();
}
}
