// [0.5.0 5.6.7 5.6.9] specs/features/0.5.0-map.md
#include <kgc/map/Ui.h>
#include <kgc/ui/UiRegistry.h>
#include "Constants.h"
#include "MapController.h"
#include "MapServicePrivate.h"
#include "TileSource.h"
namespace kgc::map {
void registerUi(kgc::ui::UiRegistry& registry, MapService& service)
{
    // 지도 플러그인은 전역 변수 대신 QML 엔진 프로퍼티로 공급을 찾아요.
    auto& source = tileSource(service);
    registry.engine.setProperty(kTileSourceProperty, QVariant::fromValue<QObject*>(&source));
    // 끌 때는 서비스가 지도 창보다 먼저 지워지므로, 공급이 사라지면 프로퍼티를 비워 매달린 포인터를 남기지 않아요.
    QObject::connect(&source, &QObject::destroyed, &registry.engine,
                     [engine = &registry.engine] { engine->setProperty(kTileSourceProperty, QVariant()); });
    new MapController(service, registry.mapLayers);
}
}
