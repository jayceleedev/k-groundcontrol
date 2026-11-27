// [0.5.1 5.7] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.9 5.9 T22 T25] specs/features/0.5.0-map.md
#include "MapController.h"
#include "Constants.h"
#include "DownloadPlan.h"
#include "Events.h"
#include "ScaleBar.h"
#include "SizeText.h"
#include "TileMath.h"
#include "Trail.h"
#include "VehicleMarkerModel.h"
#include "ZoomStep.h"
#include <kgc/app/log/Log.h>
#include <kgc/map/Errors.h>
#include <kgc/map/ModuleId.h>
#include <kgc/map/Settings.h>
#include <kgc/ui/Slots.h>
#include <kgc/vehicle/registry/VehicleRegistry.h>
#include <kgc/vehicle/states/VehicleStates.h>
#include <chrono>
namespace kgc::map {
using kgc::app::job::Job;
using kgc::vehicle::registry::VehicleRegistry;
MapController::MapController(MapService& service, kgc::ui::MapLayerSlot& layers)
    : QObject(&service), service_(service), layers_(layers), vehicles_(new VehicleMarkerModel(service.vehicles(), this))
{
    Q_ASSERT(!instance_);
    instance_ = this;
    connect(&service_, &MapService::mapTypeChanged, this, &MapController::mapTypeChanged);
    connect(&service_, &MapService::languageChanged, this, &MapController::languageChanged);
    connect(&service_, &MapService::tilePixelRatioChanged, this, &MapController::tilePixelRatioChanged);
    connect(&service_, &MapService::trailPointAdded, this, &MapController::trailPointAdded);
    connect(&service_, &MapService::trailsCleared, this, &MapController::trailsCleared);
    connect(&service_, &MapService::downloadChanged, this, &MapController::downloadChanged);
    connect(&service_, &MapService::storeSizeReady, this, [this](qint64 totalBytes, qint64 pinnedBytes) {
        storeTotalText_ = sizeText(totalBytes);
        storePinnedText_ = sizeText(pinnedBytes);
        storeError_.clear();
        emit storeChanged();
    });
    connect(&service_, &MapService::storeFailed, this, [this](kgc::app::errors::ErrorCode error) {
        storeError_ = errorText(error);
        emit storeChanged();
    });
    connect(&service_, &MapService::storeCleared, this, &MapController::refreshStoreSize);
    // 설정은 앱 설정 칸이 바꾸므로 생성 접근자의 바뀜을 그대로 넘겨요(UiSettingValues 선례).
    settings::onTileStoreLimitMbChanged(this, [this](int) { emit tileStoreLimitMbChanged(); });
    settings::onTrailMinMetersChanged(this, [this](double) { emit trailMinMetersChanged(); });
    settings::onTrailMaxPointsChanged(this, [this](int) { emit trailMaxPointsChanged(); });
    settings::onAutoFollowChanged(this, [this](bool) { emit autoFollowChanged(); });
    connect(&service_.vehicles(), &VehicleRegistry::activeChanged, this, [this] {
        bindActive();
        emit activeSysidChanged();
        emit activePositionChanged();
    });
    bindActive();
}
MapController* MapController::create(QQmlEngine* engine, QJSEngine*)
{
    Q_ASSERT(instance_ && instance_->thread() == engine->thread());
    QQmlEngine::setObjectOwnership(instance_, QQmlEngine::CppOwnership);
    return instance_;
}
void MapController::bindActive()
{
    // 활성 기체가 바뀌면 이전 기체의 위치 바뀜을 끊고 새 기체에 이어요.
    disconnect(activePosition_);
    if (auto* vehicle = service_.vehicles().active())
        activePosition_ = connect(vehicle->states(), &kgc::vehicle::states::VehicleStates::positionChanged, this,
                                  &MapController::activePositionChanged);
}
int MapController::mapType() const { return int(service_.mapType()); }
void MapController::setMapType(int value)
{
    if (value < 0 || value >= int(settings::mapTypeValues().size())) return;
    service_.setMapType(settings::MapType(value));
}
QStringList MapController::mapTypeNames() const
{
    return {QString::fromUtf8(QT_TRANSLATE_NOOP("Map", "위성")), QString::fromUtf8(QT_TRANSLATE_NOOP("Map", "하이브리드")),
            QString::fromUtf8(QT_TRANSLATE_NOOP("Map", "일반 지도"))};
}
QString MapController::language() const { return service_.language(); }
QAbstractItemModel* MapController::vehicles() const { return vehicles_; }
QAbstractItemModel* MapController::layers() const { return &layers_; }
int MapController::activeSysid() const
{
    const auto* vehicle = service_.vehicles().active();
    return vehicle ? vehicle->sysid() : -1;
}
QGeoCoordinate MapController::activeCoordinate() const
{
    const auto* vehicle = service_.vehicles().active();
    if (!vehicle) return {};
    const auto position = vehicle->states()->position();
    return isMapPosition(position.latDeg, position.lonDeg) ? QGeoCoordinate(position.latDeg, position.lonDeg) : QGeoCoordinate();
}
bool MapController::activeHasPosition() const { return activeCoordinate().isValid(); }
int MapController::minZoom() const { return kMinZoom; }
int MapController::maxZoom() const { return kMaxZoom; }
int MapController::saveViewDelayMs() const { return kSaveViewDelayMs; }
int MapController::tilePixelRatio() const { return service_.tilePixelRatio(); }
double MapController::wheelGestureTimeout() const
{
    // WheelHandler.activeTimeout은 초 단위예요.
    return std::chrono::duration<double>(kWheelGestureTimeout).count();
}
int MapController::tileStoreLimitMb() const { return settings::tileStoreLimitMb(); }
void MapController::setTileStoreLimitMb(int value) { settings::setTileStoreLimitMb(value); }
int MapController::tileStoreLimitMbMin() const { return settings::tileStoreLimitMbMin(); }
int MapController::tileStoreLimitMbMax() const { return settings::tileStoreLimitMbMax(); }
double MapController::trailMinMeters() const { return settings::trailMinMeters(); }
void MapController::setTrailMinMeters(double value) { settings::setTrailMinMeters(value); }
double MapController::trailMinMetersMin() const { return settings::trailMinMetersMin(); }
double MapController::trailMinMetersMax() const { return settings::trailMinMetersMax(); }
int MapController::trailMaxPoints() const { return settings::trailMaxPoints(); }
void MapController::setTrailMaxPoints(int value) { settings::setTrailMaxPoints(value); }
int MapController::trailMaxPointsMin() const { return settings::trailMaxPointsMin(); }
int MapController::trailMaxPointsMax() const { return settings::trailMaxPointsMax(); }
bool MapController::autoFollow() const { return settings::autoFollow(); }
void MapController::setAutoFollow(bool value) { settings::setAutoFollow(value); }
QVariantMap MapController::lastView() const
{
    const auto view = service_.lastView();
    return {{QStringLiteral("lat"), view.lat}, {QStringLiteral("lon"), view.lon}, {QStringLiteral("zoom"), view.zoom}};
}
void MapController::saveView(double lat, double lon, double zoom) { service_.saveView({lat, lon, zoom}); }
QVariantList MapController::trailOf(int sysid) const
{
    QVariantList path;
    const auto points = service_.trailOf(sysid);
    path.reserve(points.size());
    for (const auto& point : points) path.append(QVariant::fromValue(point));
    return path;
}
void MapController::clearTrails() { service_.clearTrails(); }
QVariantMap MapController::scaleFor(double latitude, double zoom, int maxPixels) const
{
    const auto length = scaleBar(metersPerPixel(latitude, zoom), maxPixels);
    return {{QStringLiteral("pixels"), length.pixels}, {QStringLiteral("label"), scaleLabel(length.meters)}};
}
QVariantMap MapController::wheelStep(double sum, bool fine, int delta, double sinceStepMs) const
{
    // 같은 이름의 멤버를 스스로 부르지 않도록 순수 함수는 이름공간을 붙여 불러요.
    const auto since = static_cast<qint64>(sinceStepMs);
    const auto wheel = kgc::map::wheelStep(sum, fine, delta, since);
    KGC_LOG_DEBUG(kCategory, MapEvent::WheelInput, "Map wheel: delta %1, fine %2, steps %3, since %4 ms", delta,
                  wheel.fine ? QStringLiteral("true") : QStringLiteral("false"), wheel.steps, since);
    return {{QStringLiteral("steps"), wheel.steps}, {QStringLiteral("sum"), wheel.sum}, {QStringLiteral("fine"), wheel.fine}};
}
double MapController::zoomStepTarget(double current, int steps, double minZoom, double maxZoom) const
{
    return kgc::map::zoomStepTarget(current, steps, minZoom, maxZoom);
}
double MapController::pinchSnapTarget(double startZoom, double endZoom, double minZoom, double maxZoom) const
{
    return kgc::map::pinchSnapTarget(startZoom, endZoom, minZoom, maxZoom);
}
void MapController::reportLayerFailed(const QString& id, const QString& error)
{
    KGC_LOG_WARN(kCategory, MapEvent::LayerFailed, "Map layer failed: %1, %2", id, error);
}
Job* MapController::downloadJob() const { return service_.downloadJob(); }
bool MapController::downloadRunning() const
{
    const auto* job = downloadJob();
    return job && (job->state() == Job::State::Pending || job->state() == Job::State::Running);
}
bool MapController::downloadFailed() const
{
    const auto* job = downloadJob();
    return job && job->state() == Job::State::Failed;
}
QString MapController::downloadResultText() const
{
    const auto* job = downloadJob();
    if (!job) return {};
    switch (job->state()) {
    case Job::State::Succeeded: return tr("다 받았어요");
    case Job::State::Failed:
    case Job::State::Cancelled: return job->errorText();
    default: return {};
    }
}
qint64 MapController::tileCount(QGeoRectangle area, int minZoom, int maxZoom) const
{
    return service_.tileCount(area, minZoom, maxZoom);
}
QString MapController::estimateText(qint64 tiles) const
{
    // 2배 타일은 같은 수에 바이트가 약 4배예요.
    return sizeText(tiles * kAvgTileBytes[std::size_t(service_.mapType())] * (tilePixelRatio() == 2 ? kHighDpiBytesFactor : 1));
}
QString MapController::planError(QGeoRectangle area, int minZoom, int maxZoom) const
{
    const auto plan = planDownload(area, minZoom, maxZoom);
    return plan ? QString() : errorText(plan.error());
}
bool MapController::startDownload(QGeoRectangle area, int minZoom, int maxZoom)
{
    const auto job = service_.startDownload(area, minZoom, maxZoom);
    downloadError_ = job ? QString() : errorText(job.error());
    emit downloadChanged();
    return bool(job);
}
void MapController::dismissDownload()
{
    downloadError_.clear();
    service_.dismissDownload();
}
void MapController::refreshStoreSize() { service_.requestStoreSize(); }
void MapController::clearStore() { service_.clearStore(); }
}
