// [0.5.1 5.6] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.5 5.6.8] specs/features/0.5.0-map.md
#include "MapServicePrivate.h"
#include "Constants.h"
#include "DownloadPlan.h"
#include "Events.h"
#include "TileMath.h"
#include "TileSource.h"
#include "TileStore.h"
#include "ZoomStep.h"
#include <kgc/app/i18n/Translator.h>
#include <kgc/app/log/Log.h>
#include <kgc/app/paths/Paths.h>
#include <kgc/map/Errors.h>
#include <kgc/map/ModuleId.h>
#include <QGuiApplication>
#include <QScreen>
#include <algorithm>
#include <cmath>
namespace kgc::map {
using kgc::app::job::Job;
void MapService::Private::decideTileRatio(bool logAlways)
{
    // 창이 놓인 화면이 아니라 연결된 화면 가운데 가장 큰 배율로 정해요(창을 옮겨도 바뀌지 않게).
    // QGuiApplication::devicePixelRatio()는 화면 신호를 다 낸 뒤에야 갱신되어 신호 안에서는 예전 값을 줘요.
    const auto screens = QGuiApplication::screens();
    double screenRatio = screens.isEmpty() ? 1.0 : 0.0;
    for (const auto* screen : screens) screenRatio = std::max(screenRatio, screen->devicePixelRatio());
    const int previous = source->tilePixelRatio();
    source->setTilePixelRatio(tilePixelRatioFor(screenRatio));
    if (logAlways || source->tilePixelRatio() != previous)
        KGC_LOG_INFO(kCategory, MapEvent::TileRatio, "Map tile ratio: %1, screen ratio: %2", source->tilePixelRatio(),
                     QString::number(screenRatio, 'g', 3));
}
MapService::MapService(kgc::app::net::Http& http, kgc::vehicle::registry::VehicleRegistry& vehicles,
                       kgc::app::i18n::Translator& translator, const QString& tileHost, QObject* parent)
    : QObject(parent), d_(std::make_unique<Private>(vehicles))
{
    auto* store = new TileStore;
    d_->store = store;
    store->moveToThread(&d_->storeThread);
    d_->source = new TileSource(http, *store, tileHost, this);
    // 지도 엔진이 생기기 전에 배수를 정해 첫 화면 타일을 두 번 받지 않아요.
    d_->decideTileRatio(true);
    connect(d_->source, &TileSource::tilePixelRatioChanged, this, &MapService::tilePixelRatioChanged);
    // 화면 신호 안에서 바로 정하지 않고 큐로 미뤄, 바뀐 화면 목록과 배율을 읽어요.
    const auto watch = [this](QScreen* screen) {
        connect(screen, &QScreen::logicalDotsPerInchChanged, this, [this] { d_->decideTileRatio(false); }, Qt::QueuedConnection);
        connect(screen, &QScreen::physicalDotsPerInchChanged, this, [this] { d_->decideTileRatio(false); }, Qt::QueuedConnection);
    };
    for (auto* screen : QGuiApplication::screens()) watch(screen);
    connect(qGuiApp, &QGuiApplication::screenAdded, this, [this, watch](QScreen* screen) {
        watch(screen);
        QMetaObject::invokeMethod(this, [this] { d_->decideTileRatio(false); }, Qt::QueuedConnection);
    });
    // 빠진 화면은 신호 뒤 바로 지워지므로 람다는 그 포인터를 받지 않아요.
    connect(qGuiApp, &QGuiApplication::screenRemoved, this, [this] { d_->decideTileRatio(false); }, Qt::QueuedConnection);
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
int MapService::tilePixelRatio() const { return d_->source->tilePixelRatio(); }
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
    d_->download = new MapDownloadJob(*d_->source, mapType(), language(), tilePixelRatio(), area, minZoom, maxZoom, *plan, this);
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
    // 0.5.0이 남긴 소수 확대(14.6 등)도 정수 층으로 열어요.
    return {settings::lastLatitude(), settings::lastLongitude(), std::round(settings::lastZoom())};
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
