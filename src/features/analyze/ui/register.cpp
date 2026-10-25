// [0.0.3 T1 T2 T3 T9 T12 T13 T14] specs/features/0.0.3-windows.md
#include <kgc/analyze/Ui.h>
#include <kgc/analyze/AppLogService.h>
#include <kgc/ui/UiRegistry.h>
#include "AppLogController.h"
#include <QQmlContext>
namespace kgc::analyze {
void registerUi(kgc::ui::UiRegistry& registry, AppLogService& service)
{
    auto* controller = new AppLogController(service, registry.engine);
    registry.engine.rootContext()->setContextProperty("appLogController", controller);
    const auto result = registry.bottomActions.registerAction("appLog", 20, "logs",
        QT_TRANSLATE_NOOP("BottomAction", "앱 로그"), controller, "open");
    Q_ASSERT(result.has_value());
}
}
