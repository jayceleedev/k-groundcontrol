// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#include "ShellController.h"
#include <kgc/shell/ShellService.h>
#include <kgc/shell/Settings.h>
#include <kgc/ui/Navigator.h>
#include <QGuiApplication>
namespace kgc::shell {
ShellController::ShellController(ShellService& service, kgc::ui::Navigator& navigator)
    : QObject(&service), service_(service)
{
    bindScreen(QGuiApplication::primaryScreen());
    settings::onLeftMenuCollapsedChanged(this, [this](bool collapsed) {
        service_.menuCollapsed(collapsed);
        emit leftMenuCollapsedChanged();
    });
    connect(&navigator, &kgc::ui::Navigator::openRequested, this,
        [this](const QString& id, const QString& arg) {
            openPanel(id, arg);
            emit panelArgRequested(id, arg);
        });
}
QString ShellController::activePanelId() const { return activePanelId_; }
QString ShellController::panelArg() const { return panelArg_; }
bool ShellController::leftMenuCollapsed() const { return settings::leftMenuCollapsed(); }
QSize ShellController::availableScreenSize() const { return availableScreenSize_; }
void ShellController::setLeftMenuCollapsed(bool collapsed) { settings::setLeftMenuCollapsed(collapsed); }
void ShellController::attachWindow(QQuickWindow* window)
{
    disconnect(screenConnection_);
    window_ = window;
    bindScreen(window ? window->screen() : QGuiApplication::primaryScreen());
    if (window) screenConnection_ = connect(window, &QWindow::screenChanged, this, &ShellController::bindScreen);
}
void ShellController::bindScreen(QScreen* screen)
{
    disconnect(geometryConnection_);
    if (!screen) return;
    availableScreenSize_ = screen->availableGeometry().size();
    emit availableScreenSizeChanged();
    geometryConnection_ = connect(screen, &QScreen::availableGeometryChanged, this, [this](const QRect& geometry) {
        availableScreenSize_ = geometry.size();
        emit availableScreenSizeChanged();
    });
}
void ShellController::togglePanel(const QString& id)
{
    openPanel(activePanelId_ == id ? QString{} : id, {});
}
void ShellController::openPanel(const QString& id, const QString& arg)
{
    if (activePanelId_ != id) {
        if (!activePanelId_.isEmpty()) service_.panelClosed();
        if (!id.isEmpty()) service_.panelOpened(id);
    }
    activePanelId_ = id;
    panelArg_ = arg;
    emit activePanelChanged();
}
void ShellController::windowShown(int width, int height) { service_.windowShown(width, height); }
}
