// [0.0.4 T4] specs/features/0.0.4-righttabs.md
#include <kgc/preflight/Ui.h>
#include <kgc/ui/UiRegistry.h>
namespace kgc::preflight {
void registerUi(kgc::ui::UiRegistry& registry)
{
    const auto result = registry.rightTabs.registerTab("preflight", 50, QT_TRANSLATE_NOOP("RightTab", "점검"),
        QUrl("qrc:/qt/qml/Kgc/Preflight/PreflightTab.qml"), kgc::ui::RightTabPlacement::Tab);
    Q_ASSERT(result.has_value());
}
}
