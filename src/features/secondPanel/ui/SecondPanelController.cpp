// [0.0.3 T1 T2 T3 T9 T12 T13 T14] specs/features/0.0.3-windows.md
#include "SecondPanelController.h"
#include <kgc/secondPanel/SecondPanelService.h>
#include <kgc/secondPanel/Settings.h>
#include <kgc/ui/WindowPlacement.h>
#include "Constants.h"
#include <QQmlComponent>
#include <QQmlEngine>
#include <QGuiApplication>
#include <QScreen>
namespace kgc::secondPanel {
SecondPanelController::SecondPanelController(SecondPanelService& service, QQmlEngine& engine)
    : QObject(&service), service_(service), engine_(engine)
{
    connect(&service_, &SecondPanelService::arrangementChanged, this, &SecondPanelController::arrangementChanged);
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this] {
        savePlacement();
        if (window_) window_->close();
    });
}
QQuickWindow* SecondPanelController::window() const { return window_; }
int SecondPanelController::saveDelayMs() const { return kSaveDelayMs; }
int SecondPanelController::arrangementIndex() const
{
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
    if (!window_) {
        QQmlComponent component(&engine_, QUrl("qrc:/qt/qml/Kgc/SecondPanel/SecondPanelWindow.qml"));
        auto* object = component.create(engine_.rootContext());
        window_ = qobject_cast<QQuickWindow*>(object);
        if (!window_) { delete object; return; }
        window_->QObject::setParent(this);
        window_->setTransientParent(nullptr);
        const auto screens = QGuiApplication::screens();
        QList<kgc::ui::ScreenInfo> info;
        for (auto* screen : screens) info.append({screen->name(), screen->availableGeometry()});
        const auto placement = kgc::ui::restorePlacement(settings::windowPlacement(), info, kDefaultSize);
        window_->setScreen(screens[placement.screenIndex]);
        window_->setGeometry(placement.geometry);
        emit windowChanged();
    }
    if (window_->visibility() == QWindow::Minimized) window_->showNormal();
    else window_->show();
    window_->raise();
    window_->requestActivate();
}
void SecondPanelController::savePlacement()
{
    if (!window_ || window_->visibility() != QWindow::Windowed || !window_->screen()) return;
    const auto* screen = window_->screen();
    const auto relative = window_->geometry().translated(-screen->availableGeometry().topLeft());
    settings::setWindowPlacement(kgc::ui::savePlacement(screen->name(), relative));
}
}
