// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#include <kgc/appSettings/Ui.h>
#include <kgc/appSettings/AppSettingsService.h>
#include <kgc/ui/UiRegistry.h>
#include "AppSettingsController.h"
#include <QQmlContext>
namespace kgc::appSettings {
void registerUi(kgc::ui::UiRegistry& registry, AppSettingsService& service)
{
    const auto result = registry.panels.registerPanel({"appSettings", 15, 4,
        QT_TRANSLATE_NOOP("Menu", "앱 설정"), QUrl("qrc:/kgc/icons/menu/appSettings.svg"),
        QUrl("qrc:/qt/qml/Kgc/AppSettings/AppSettingsPanel.qml")});
    Q_ASSERT(result.has_value());
    registry.engine.rootContext()->setContextProperty("appSettingsController", new AppSettingsController(service));
}
}
