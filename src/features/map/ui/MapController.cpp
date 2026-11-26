// [0.5.0 5.6.9] specs/features/0.5.0-map.md
#include "MapController.h"
#include "Constants.h"
#include "Events.h"
#include "ScaleBar.h"
#include "SizeText.h"
#include "TileMath.h"
#include <kgc/app/log/Log.h>
#include <kgc/map/Errors.h>
#include <kgc/map/ModuleId.h>
#include <kgc/ui/Slots.h>
namespace kgc::map {
MapController::MapController(MapService& service, kgc::ui::MapLayerSlot& layers)
    : QObject(&service), service_(service), layers_(layers)
{
    Q_ASSERT(!instance_);
    instance_ = this;
    connect(&service_, &MapService::mapTypeChanged, this, &MapController::mapTypeChanged);
    connect(&service_, &MapService::languageChanged, this, &MapController::languageChanged);
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
}
MapController* MapController::create(QQmlEngine* engine, QJSEngine*)
{
    Q_ASSERT(instance_ && instance_->thread() == engine->thread());
    QQmlEngine::setObjectOwnership(instance_, QQmlEngine::CppOwnership);
    return instance_;
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
QAbstractItemModel* MapController::layers() const { return &layers_; }
int MapController::minZoom() const { return kMinZoom; }
int MapController::maxZoom() const { return kMaxZoom; }
QVariantMap MapController::lastView() const
{
    const auto view = service_.lastView();
    return {{QStringLiteral("lat"), view.lat}, {QStringLiteral("lon"), view.lon}, {QStringLiteral("zoom"), view.zoom}};
}
void MapController::saveView(double lat, double lon, double zoom) { service_.saveView({lat, lon, zoom}); }
QVariantMap MapController::scaleFor(double latitude, double zoom, int maxPixels) const
{
    const auto length = scaleBar(metersPerPixel(latitude, zoom), maxPixels);
    return {{QStringLiteral("pixels"), length.pixels}, {QStringLiteral("label"), scaleLabel(length.meters)}};
}
void MapController::refreshStoreSize() { service_.requestStoreSize(); }
void MapController::clearStore() { service_.clearStore(); }
void MapController::reportLayerFailed(const QString& id, const QString& error)
{
    KGC_LOG_WARN(kCategory, MapEvent::LayerFailed, "Map layer failed: %1, %2", id, error);
}
}
