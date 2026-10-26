// [0.0.4 T3 T4] specs/features/0.0.4-righttabs.md
#include <kgc/vehicleInfo/Ui.h>
#include <kgc/ui/UiRegistry.h>
namespace kgc::vehicleInfo {
void registerUi(kgc::ui::UiRegistry& registry)
{
    using kgc::ui::RightTabPlacement;
    const auto add = [&registry](const char* id, int order, const char* titleKey, const char* file,
                                 RightTabPlacement placement) {
        const auto result = registry.rightTabs.registerTab(id, order, titleKey,
            QUrl(QStringLiteral("qrc:/qt/qml/Kgc/VehicleInfo/") + QLatin1StringView(file)), placement);
        Q_ASSERT(result.has_value());
    };
    add("instrument", 0, QT_TRANSLATE_NOOP("RightTab", "계기"), "InstrumentView.qml", RightTabPlacement::Top);
    add("overview", 10, QT_TRANSLATE_NOOP("RightTab", "개요"), "OverviewTab.qml", RightTabPlacement::Tab);
    add("detail", 20, QT_TRANSLATE_NOOP("RightTab", "상세"), "DetailTab.qml", RightTabPlacement::Tab);
    add("messages", 30, QT_TRANSLATE_NOOP("RightTab", "메시지"), "MessagesTab.qml", RightTabPlacement::Tab);
}
}
