// [0.2.0 T14] specs/features/0.2.0-fcflash.md
#include <kgc/fcFlash/Ui.h>
#include <kgc/ui/UiRegistry.h>
#include "FcFlashController.h"
namespace kgc::fcFlash {
void registerUi(kgc::ui::UiRegistry& registry, FcFlashService& service)
{
    const auto result = registry.panels.registerPanel({"fcFlash", 1, 1,
        QT_TRANSLATE_NOOP("Menu", "펌웨어 세팅"), QUrl("qrc:/kgc/icons/menu/fcFlash.svg"),
        QUrl("qrc:/qt/qml/Kgc/FcFlash/FcFlashPanel.qml")});
    Q_ASSERT(result.has_value());
    new FcFlashController(service);
}
}
