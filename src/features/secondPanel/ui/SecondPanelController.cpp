// [0.0.3 T9 T12 T13] specs/features/0.0.3-windows.md
#include "SecondPanelController.h"
#include <kgc/secondPanel/SecondPanelService.h>
#include <kgc/secondPanel/Settings.h>
#include "Constants.h"
namespace kgc::secondPanel {
SecondPanelController::SecondPanelController(SecondPanelService& service, QQmlEngine& engine)
    : QObject(&service), service_(service),
      window_(engine, QUrl("qrc:/qt/qml/Kgc/SecondPanel/SecondPanelWindow.qml"), kDefaultSize,
          [] { return settings::windowPlacement(); },
          [](const QString& value) { settings::setWindowPlacement(value); }, this)
{
    connect(&service_, &SecondPanelService::arrangementChanged, this, &SecondPanelController::arrangementChanged);
    connect(&window_, &kgc::ui::SeparateWindow::windowChanged, this, &SecondPanelController::windowChanged);
}
QQuickWindow* SecondPanelController::window() const { return window_.window(); }
int SecondPanelController::saveDelayMs() const { return kSaveDelayMs; }
int SecondPanelController::arrangementIndex() const
{
    // 창의 배치 이름 목록도 arrangementValues() 순서이므로 같은 위치로 선택을 연결해요.
    return settings::arrangementValues().indexOf(settings::toString(service_.arrangement()));
}
QVariantList SecondPanelController::cells() const
{
    QVariantList values;
    for (const auto& rect : service_.cells()) values.append(rect);
    return values;
}
void SecondPanelController::chooseArrangement(int index)
{
    const auto values = settings::arrangementValues();
    if (index >= 0 && index < values.size()) service_.setArrangement(settings::fromString(values[index]));
}
void SecondPanelController::open()
{
    window_.open();
}
void SecondPanelController::savePlacement() { window_.savePlacement(); }
}
