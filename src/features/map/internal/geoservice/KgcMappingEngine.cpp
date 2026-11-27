// [0.5.1 5.5] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.7] specs/features/0.5.0-map.md
#include "KgcMappingEngine.h"
#include "Constants.h"
#include "KgcTileCache.h"
#include "KgcTileFetcher.h"
#include "TileSource.h"
#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QtLocation/private/qgeomaptype_p.h>
namespace kgc::map {
KgcMappingEngine::KgcMappingEngine(TileSource& source, QGeoServiceProvider::Error* error, QString* errorString)
{
    QGeoCameraCapabilities capabilities;
    capabilities.setMinimumZoomLevel(kMinZoom);
    capabilities.setMaximumZoomLevel(kMaxZoom);
    capabilities.setSupportsBearing(false);
    capabilities.setSupportsTilting(false);
    capabilities.setOverzoomEnabled(false);
    setCameraCapabilities(capabilities);
    setTileSize(QSize(kTileSize, kTileSize));
    // id는 settings::MapType 순서 + 1이에요. 이름은 화면에 쓰지 않아요(화면 이름은 MapController.mapTypeNames).
    const QByteArray plugin("kgc");
    setSupportedMapTypes({
        QGeoMapType(QGeoMapType::SatelliteMapDay, QStringLiteral("Satellite"), {}, false, false, 1, plugin, capabilities),
        QGeoMapType(QGeoMapType::HybridMap, QStringLiteral("Hybrid"), {}, false, false, 2, plugin, capabilities),
        QGeoMapType(QGeoMapType::StreetMap, QStringLiteral("Road"), {}, false, false, 3, plugin, capabilities)});
    // 디스크 캐시는 TileStore가 맡으므로 Qt 캐시는 메모리만 써요. fetcher보다 먼저 정해요.
    setCacheHint(QAbstractGeoTileCache::MemoryCache);
    setTileCache(new KgcTileCache(source.tilePixelRatio(), this));
    setTileFetcher(new KgcTileFetcher(source, this));
    // 언어가 바뀌면 모든 지도가 보이는 타일을 새 판으로 다시 청해요(하이브리드·일반은 서버, 위성은 저장소에서).
    connect(&source, &TileSource::languageChanged, this, [this] { setTileVersion(tileVersion() + 1); });
    // 배수가 바뀌거나 대신 쓴 그림이 낡으면 보이는 칸을 지금 배수로 다시 청해요(저장소에 있으면 저장소에서).
    connect(&source, &TileSource::tilePixelRatioChanged, this, [this] { setTileVersion(tileVersion() + 1); });
    connect(&source, &TileSource::fallbackStale, this, [this] { setTileVersion(tileVersion() + 1); });
    connect(&source, &TileSource::onlineAgain, this, &KgcMappingEngine::refreshVisible);
    *error = QGeoServiceProvider::NoError;
    errorString->clear();
}
QGeoMap* KgcMappingEngine::createMap()
{
    // 기본은 이웃 확대 층까지 미리 받아 서버 요청이 몇 배로 늘어요. 보이는 타일만 받게 꺼요.
    auto* map = new QGeoTiledMap(this, nullptr);
    map->setPrefetchStyle(QGeoTiledMap::NoPrefetching);
    maps_.removeAll(nullptr);
    maps_.append(map);
    return map;
}
void KgcMappingEngine::refreshVisible()
{
    for (const auto& map : std::as_const(maps_))
        if (map) map->clearScene(map->activeMapType().mapId());
}
}
