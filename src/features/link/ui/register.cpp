// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
#include <kgc/link/Ui.h>
#include <kgc/ui/UiRegistry.h>
#include "LinkController.h"
namespace kgc::link {
void registerUi(kgc::ui::UiRegistry& registry, LinkService& service)
{
    new LinkController(service);
    const auto top = [&registry](const char* id, int order, const char* file) {
        const auto result = registry.topbar.registerItem(id, order, "right", 6,
            QUrl(QStringLiteral("qrc:/qt/qml/Kgc/Link/") + QLatin1StringView(file)));
        Q_ASSERT(result.has_value());
    };
    top("link.port", 12, "LinkPort.qml");
    top("link.baud", 13, "LinkBaud.qml");
    top("link.connect", 14, "LinkConnect.qml");
    const auto result = registry.statusChips.registerItem("link.connection", 1, "left",
        QUrl("qrc:/qt/qml/Kgc/Link/LinkConnection.qml"));
    Q_ASSERT(result.has_value());
}
}
