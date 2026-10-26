// [0.0.4 T4] specs/features/0.0.4-righttabs.md
#include <kgc/logs/Ui.h>
#include <kgc/logs/LogsViewer.h>
#include <kgc/ui/UiRegistry.h>
namespace kgc::logs {
void registerUi(kgc::ui::UiRegistry& registry, LogsViewer& viewer)
{
    // 뷰어는 아직 상태가 없어 화면에 넘길 것이 없어요(0.1.x에서 컨트롤러가 받아요).
    Q_UNUSED(viewer);
    const auto result = registry.rightTabs.registerTab("logs", 40, QT_TRANSLATE_NOOP("RightTab", "로그"),
        QUrl("qrc:/qt/qml/Kgc/Logs/LogsTab.qml"), kgc::ui::RightTabPlacement::Tab);
    Q_ASSERT(result.has_value());
}
}
