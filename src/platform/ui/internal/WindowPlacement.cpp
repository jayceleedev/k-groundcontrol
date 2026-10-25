// [0.0.3 T11] specs/features/0.0.3-windows.md
#include <kgc/ui/WindowPlacement.h>
#include "Constants.h"
#include <algorithm>
#include <array>
namespace kgc::ui {
Placement restorePlacement(const QString& saved, const QList<ScreenInfo>& screens, QSize defaultSize)
{
    Q_ASSERT(!screens.isEmpty());
    int screenIndex = 0;
    QRect geometry(QPoint{}, defaultSize);
    bool restored = false;
    const auto parts = saved.split('|');
    if (parts.size() >= 5) {
        std::array<int, 4> values{};
        bool valid = true;
        for (int i = 0; i < 4; ++i) {
            bool ok = false;
            values[i] = parts[parts.size() - 4 + i].toInt(&ok);
            valid &= ok;
        }
        const auto name = parts.mid(0, parts.size() - 4).join('|');
        if (valid && values[2] > 0 && values[3] > 0) {
            for (int i = 0; i < screens.size(); ++i) {
                if (screens[i].name != name) continue;
                screenIndex = i;
                geometry = QRect(values[0], values[1], values[2], values[3]);
                restored = true;
                break;
            }
        }
    }
    const auto area = screens[screenIndex].available;
    if (!restored) geometry.moveTopLeft({(area.width() - defaultSize.width()) / 2,
                                         (area.height() - defaultSize.height()) / 2});
    geometry.setSize({std::min(geometry.width(), area.width()),
                      std::min(geometry.height(), area.height() - kTitleBarMargin)});
    geometry.moveLeft(std::clamp(geometry.x(), 0, area.width() - geometry.width()));
    geometry.moveTop(std::clamp(geometry.y(), kTitleBarMargin, area.height() - geometry.height()));
    geometry.translate(area.topLeft());
    return {screenIndex, geometry};
}
QString savePlacement(const QString& screenName, QRect geometryInScreen)
{
    return QString("%1|%2|%3|%4|%5").arg(screenName).arg(geometryInScreen.x())
        .arg(geometryInScreen.y()).arg(geometryInScreen.width()).arg(geometryInScreen.height());
}
}
