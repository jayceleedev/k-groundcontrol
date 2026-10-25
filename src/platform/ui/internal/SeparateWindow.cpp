// [0.0.3 T3 T9 T12 T13] specs/features/0.0.3-windows.md
#include <kgc/ui/SeparateWindow.h>
#include <kgc/ui/WindowPlacement.h>
#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QScreen>
#include <utility>

namespace kgc::ui {
SeparateWindow::SeparateWindow(QQmlEngine& engine, QUrl source, QSize defaultSize,
    std::function<QString()> readPlacement, std::function<void(const QString&)> writePlacement,
    QObject* parent)
    : QObject(parent), engine_(engine), source_(std::move(source)), defaultSize_(defaultSize),
      readPlacement_(std::move(readPlacement)), writePlacement_(std::move(writePlacement))
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this] {
        savePlacement();
        if (window_) window_->close();
    });
}
QQuickWindow* SeparateWindow::window() const { return window_; }
bool SeparateWindow::open()
{
    if (!window_) {
        const auto screens = QGuiApplication::screens();
        if (screens.isEmpty()) return false;
        QQmlComponent component(&engine_, source_);
        auto* object = component.createWithInitialProperties({{"visible", false}}, engine_.rootContext());
        window_ = qobject_cast<QQuickWindow*>(object);
        if (!window_) { delete object; return false; }
        window_->QObject::setParent(this);
        window_->setTransientParent(nullptr);
        QList<ScreenInfo> info;
        for (auto* screen : screens) info.append({screen->name(), screen->availableGeometry()});
        const auto placement = restorePlacement(readPlacement_(), info, defaultSize_);
        window_->setScreen(screens[placement.screenIndex]);
        window_->setGeometry(placement.geometry);
        emit windowChanged();
    }
    if (window_->visibility() == QWindow::Minimized) window_->showNormal();
    else window_->show();
    window_->raise();
    window_->requestActivate();
    return true;
}
void SeparateWindow::savePlacement()
{
    if (!window_ || window_->visibility() != QWindow::Windowed || !window_->screen()) return;
    const auto* screen = window_->screen();
    const auto relative = window_->geometry().translated(-screen->availableGeometry().topLeft());
    writePlacement_(kgc::ui::savePlacement(screen->name(), relative));
}
}
