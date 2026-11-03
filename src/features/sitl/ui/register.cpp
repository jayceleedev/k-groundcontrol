// [0.4.0 5.4] specs/features/0.4.0-sitl.md
#include <kgc/sitl/Ui.h>
#include <kgc/ui/UiRegistry.h>
#include "SitlController.h"
namespace kgc::sitl {
void registerUi(kgc::ui::UiRegistry& registry, SitlService& service)
{
    const auto result = registry.panels.registerPanel({"sitl", 2, 1,
        QT_TRANSLATE_NOOP("Menu", "시뮬레이션"), QUrl("qrc:/kgc/icons/menu/sitl.svg"),
        QUrl("qrc:/qt/qml/Kgc/Sitl/SitlPanel.qml")});
    Q_ASSERT(result.has_value());
    new SitlController(service);
}
}
