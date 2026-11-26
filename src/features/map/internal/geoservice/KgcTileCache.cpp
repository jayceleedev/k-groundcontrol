// [0.5.0 5.6.7] specs/features/0.5.0-map.md
#include "KgcTileCache.h"
#include "Constants.h"
namespace kgc::map {
KgcTileCache::KgcTileCache(QObject* parent) : QGeoFileTileCache(QString(), parent) {}
void KgcTileCache::init()
{
    setMaxDiskUsage(0);
    setMaxMemoryUsage(kTileMemoryCacheBytes);
    setExtraTextureUsage(kTileExtraTextureBytes);
}
void KgcTileCache::clearAll()
{
    textureCache_.clear();
    memoryCache_.clear();
    diskCache_.clear();
}
}
