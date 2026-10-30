// [0.0.4 T41 T42 T43] specs/features/0.0.4-righttabs.md
#include <kgc/reference/Ui.h>
#include <kgc/reference/ReferenceService.h>
#include <kgc/ui/UiRegistry.h>
namespace kgc::reference {
void registerUi(kgc::ui::UiRegistry& registry, ReferenceService& service)
{
    Q_UNUSED(service);
    const auto result = registry.panels.registerPanel({"reference", 15, 4,
        QT_TRANSLATE_NOOP("Menu", "참고"), QUrl("qrc:/kgc/icons/menu/reference.svg"),
        QUrl("qrc:/qt/qml/Kgc/Reference/ReferencePanel.qml")});
    Q_ASSERT(result.has_value());
}
}
