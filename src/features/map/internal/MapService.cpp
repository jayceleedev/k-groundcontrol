// [0.5.0 5.6.5 5.6.8] specs/features/0.5.0-map.md
#include "MapServicePrivate.h"
#include "Constants.h"
#include "DownloadPlan.h"
#include "TileMath.h"
#include "TileSource.h"
#include "TileStore.h"
#include <kgc/app/i18n/Translator.h>
#include <kgc/app/paths/Paths.h>
#include <kgc/map/Errors.h>
#include <algorithm>
namespace kgc::map {
using kgc::app::job::Job;
MapService::MapService(kgc::app::net::Http& http, kgc::vehicle::registry::VehicleRegistry& vehicles,
                       kgc::app::i18n::Translator& translator, const QString& tileHost, QObject* parent)
    : QObject(parent), d_(std::make_unique<Private>(vehicles))
{
    auto* store = new TileStore;
    d_->store = store;
    store->moveToThread(&d_->storeThread);
    d_->source = new TileSource(http, *store, tileHost, this);
    d_->source->setLanguage(translator.language());
    connect(&translator, &kgc::app::i18n::Translator::languageChanged, this,
            [this, &translator] { d_->source->setLanguage(translator.language()); });
    connect(d_->source, &TileSource::languageChanged, this, &MapService::languageChanged);
    connect(&d_->trails, &TrailBook::pointAdded, this, &MapService::trailPointAdded);
    connect(&d_->trails, &TrailBook::cleared, this, &MapService::trailsCleared);
    connect(store, &TileStore::sized, this, [this](qint64 totalBytes, qint64 pinnedBytes) {
        d_->storeBroken = false;
        emit storeSizeReady(totalBytes, pinnedBytes);
    });
    connect(store, &TileStore::cleared, this, &MapService::storeCleared);
    connect(store, &TileStore::failed, this, [this](kgc::app::errors::ErrorCode error) {
        d_->storeBroken = true;
        emit storeFailed(error);
    });
    settings::onTileStoreLimitMbChanged(this, [store](int megabytes) {
        const qint64 bytes = qint64(megabytes) * kBytesPerKilobyte * kBytesPerKilobyte;
        QMetaObject::invokeMethod(store, [store, bytes] { store->setLimit(bytes); });
    });
    const auto path = kgc::app::paths::Paths::cacheTiles() + QStringLiteral("/tiles.sqlite");
    const qint64 limit = qint64(settings::tileStoreLimitMb()) * kBytesPerKilobyte * kBytesPerKilobyte;
    connect(&d_->storeThread, &QThread::started, store, [store, path, limit] { store->open(path, limit); });
    connect(&d_->storeThread, &QThread::finished, store, &QObject::deleteLater);
    // 열기 실패를 놓치지 않게 저장소 시그널을 모두 이은 뒤 스레드를 시작해요.
    d_->storeThread.start();
}
MapService::~MapService()
{
    QObject::disconnect(d_->store, nullptr, nullptr, nullptr);
    d_->storeThread.quit();
    d_->storeThread.wait();
}
settings::MapType MapService::mapType() const { return settings::mapType(); }
void MapService::setMapType(settings::MapType type)
{
    if (type == settings::mapType()) return;
    settings::setMapType(type);
    emit mapTypeChanged();
}
QString MapService::language() const { return d_->source->language(); }
kgc::vehicle::registry::VehicleRegistry& MapService::vehicles() const { return d_->vehicles; }
qint64 MapService::tileCount(const QGeoRectangle& area, int minZoom, int maxZoom) const
{
    return kgc::map::tileCount(area, minZoom, maxZoom);
}
kgc::app::errors::Result<Job*> MapService::startDownload(const QGeoRectangle& area, int minZoom, int maxZoom)
{
    const auto plan = planDownload(area, minZoom, maxZoom);
    if (!plan) return std::unexpected(plan.error());
    // 저장소가 고장이면 받아도 남지 않으므로 시작하지 않아요.
    if (d_->storeBroken) return std::unexpected(make(MapError::StoreFailed));
    if (const auto* job = downloadJob(); job && (job->state() == Job::State::Pending || job->state() == Job::State::Running))
        return std::unexpected(make(MapError::DownloadBusy));
    if (d_->download) d_->download->deleteLater();
    d_->download = new MapDownloadJob(*d_->source, mapType(), language(), area, minZoom, maxZoom, *plan, this);
    connect(d_->download->job(), &Job::finished, this, &MapService::downloadChanged);
    d_->download->start();
    emit downloadChanged();
    return d_->download->job();
}
Job* MapService::downloadJob() const { return d_->download ? d_->download->job() : nullptr; }
void MapService::dismissDownload()
{
    if (d_->download) d_->download->deleteLater();
    d_->download = nullptr;
    emit downloadChanged();
}
void MapService::requestStoreSize()
{
    auto* store = d_->store;
    QMetaObject::invokeMethod(store, [store] { store->size(); });
}
void MapService::clearStore()
{
    auto* store = d_->store;
    QMetaObject::invokeMethod(store, [store] { store->clear(); });
}
QList<QGeoCoordinate> MapService::trailOf(int sysid) const { return d_->trails.trailOf(sysid); }
void MapService::clearTrails() { d_->trails.clear(); }
SavedView MapService::lastView() const
{
    return {settings::lastLatitude(), settings::lastLongitude(), settings::lastZoom()};
}
void MapService::saveView(SavedView view)
{
    // 범위 밖 쓰기는 거부되므로 설정 범위로 잘라서 써요.
    settings::setLastLatitude(std::clamp(view.lat, settings::lastLatitudeMin(), settings::lastLatitudeMax()));
    settings::setLastLongitude(std::clamp(view.lon, settings::lastLongitudeMin(), settings::lastLongitudeMax()));
    settings::setLastZoom(std::clamp(view.zoom, settings::lastZoomMin(), settings::lastZoomMax()));
}
TileSource& tileSource(MapService& service) { return *service.d_->source; }
}
