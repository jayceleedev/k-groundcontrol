// [0.4.0 10] specs/features/0.4.0-sitl.md
// [0.3.0 6.8] specs/features/0.3.0-params.md
#include <kgc/params/Ui.h>
#include <kgc/ui/UiRegistry.h>
#include "ParamsController.h"
namespace kgc::params {
void registerUi(kgc::ui::UiRegistry& registry, kgc::vehicle::registry::VehicleRegistry& vehicles)
{
    const auto result = registry.panels.registerPanel({"params", 15, 4,
        QT_TRANSLATE_NOOP("Menu", "수동 기체 설정"), QUrl("qrc:/kgc/icons/menu/params.svg"),
        QUrl("qrc:/qt/qml/Kgc/Params/ParamsPanel.qml")});
    Q_ASSERT(result.has_value());
    new ParamsController(vehicles);
}
}
