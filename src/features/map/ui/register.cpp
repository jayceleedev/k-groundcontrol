// [0.5.0 5.6.7 5.6.9 T24] specs/features/0.5.0-map.md
#include <kgc/map/Ui.h>
#include <kgc/ui/Slots.h>
#include <kgc/ui/UiRegistry.h>
#include "Constants.h"
#include "MapController.h"
#include "MapServicePrivate.h"
#include "TileSource.h"
namespace kgc::map {
void registerUi(kgc::ui::UiRegistry& registry, MapService& service)
{
    // 궤적·홈·기체 레이어 사이를 비워 미션·유도·트래커 레이어가 끼어들 수 있게 해요.
    [[maybe_unused]] const auto trails = registry.mapLayers.registerLayer(
        QStringLiteral("mapTrails"), 10, QUrl(QStringLiteral("qrc:/qt/qml/Kgc/Map/TrailLayer.qml")));
    Q_ASSERT(trails);
    [[maybe_unused]] const auto homes = registry.mapLayers.registerLayer(
        QStringLiteral("mapHomes"), 20, QUrl(QStringLiteral("qrc:/qt/qml/Kgc/Map/HomeLayer.qml")));
    Q_ASSERT(homes);
    [[maybe_unused]] const auto vehicles = registry.mapLayers.registerLayer(
        QStringLiteral("mapVehicles"), 30, QUrl(QStringLiteral("qrc:/qt/qml/Kgc/Map/VehicleLayer.qml")));
    Q_ASSERT(vehicles);
    // 지도 플러그인은 전역 변수 대신 QML 엔진 프로퍼티로 공급을 찾아요.
    auto& source = tileSource(service);
    registry.engine.setProperty(kTileSourceProperty, QVariant::fromValue<QObject*>(&source));
    // 끌 때는 서비스가 지도 창보다 먼저 지워지므로, 공급이 사라지면 프로퍼티를 비워 매달린 포인터를 남기지 않아요.
    QObject::connect(&source, &QObject::destroyed, &registry.engine,
                     [engine = &registry.engine] { engine->setProperty(kTileSourceProperty, QVariant()); });
    new MapController(service, registry.mapLayers);
}
}
