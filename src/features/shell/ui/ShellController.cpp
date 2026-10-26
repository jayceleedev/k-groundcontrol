// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
// [0.0.4 T25a T25b T25c] specs/features/0.0.4-righttabs.md
#include "ShellController.h"
#include <kgc/shell/ShellService.h>
#include <kgc/shell/Settings.h>
#include <kgc/ui/Navigator.h>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QScreen>
namespace kgc::shell {
ShellController::ShellController(ShellService& service, kgc::ui::Navigator& navigator)
    : QObject(&service), service_(service)
{
    bindScreen(QGuiApplication::primaryScreen());
    settings::onLeftMenuCollapsedChanged(this, [this](bool collapsed) {
        service_.menuCollapsed(collapsed);
        emit leftMenuCollapsedChanged();
    });
    settings::onLeftMenuWidthChanged(this, [this](int) { emit leftMenuWidthChanged(); });
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
int ShellController::leftMenuWidth() const { return settings::leftMenuWidth(); }
int ShellController::leftMenuWidthMin() const { return settings::leftMenuWidthMin(); }
int ShellController::leftMenuWidthMax() const { return settings::leftMenuWidthMax(); }
void ShellController::setLeftMenuWidth(int width) { settings::setLeftMenuWidth(width); }
void ShellController::attachWindow(QQuickWindow* window)
{
    disconnect(screenConnection_);
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
