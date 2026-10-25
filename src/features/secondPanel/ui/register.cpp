// [0.0.3 T2] specs/features/0.0.3-windows.md
#include <kgc/secondPanel/Ui.h>
#include <kgc/secondPanel/SecondPanelService.h>
#include <kgc/ui/UiRegistry.h>
#include "SecondPanelController.h"
#include <QQmlContext>
namespace kgc::secondPanel {
void registerUi(kgc::ui::UiRegistry& registry, SecondPanelService& service)
{
    auto* controller = new SecondPanelController(service, registry.engine);
    registry.engine.rootContext()->setContextProperty("secondPanelController", controller);
    const auto result = registry.bottomActions.registerAction("secondPanel", 10, "layout-dashboard",
        QT_TRANSLATE_NOOP("BottomAction", "세컨드 패널"), controller, "open");
    Q_ASSERT(result.has_value());
}
}
