// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
#include <kgc/uiGallery/Ui.h>
#include <kgc/uiGallery/UiGalleryService.h>
#include <kgc/ui/UiRegistry.h>
namespace kgc::uiGallery {
void registerUi(kgc::ui::UiRegistry& registry, UiGalleryService& service)
{
    Q_UNUSED(service);
    const auto result = registry.panels.registerPanel({"uiGallery", 16, 5,
        QT_TRANSLATE_NOOP("Menu", "UI 부품 목록"), QUrl("qrc:/kgc/icons/menu/uiGallery.svg"),
        QUrl("qrc:/qt/qml/Kgc/UiGallery/UiGalleryPanel.qml")});
    Q_ASSERT(result.has_value());
}
}
