// [0.0.3 T1 T2 T3 T9 T12 T13 T14] specs/features/0.0.3-windows.md
#include "AppLogController.h"
#include <kgc/analyze/AppLogService.h>
#include <kgc/analyze/Settings.h>
#include <kgc/ui/WindowPlacement.h>
#include "Constants.h"
#include <kgc/analyze/ModuleId.h>
#include <kgc/app/log/Log.h>
#include "Events.h"
#include <QQmlComponent>
#include <QQmlEngine>
#include <QGuiApplication>
#include <QScreen>
namespace kgc::analyze {
AppLogController::AppLogController(AppLogService& service, QQmlEngine& engine)
    : QObject(&service), service_(service), engine_(engine)
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this] {
        savePlacement();
        if (window_) window_->close();
    });
}
QQuickWindow* AppLogController::window() const { return window_; }
int AppLogController::saveDelayMs() const { return kSaveDelayMs; }
QAbstractListModel* AppLogController::model() const { return service_.model(); }
void AppLogController::open()
{
    if (!window_) {
        QQmlComponent component(&engine_, QUrl("qrc:/qt/qml/Kgc/Analyze/AppLogWindow.qml"));
        auto* object = component.create(engine_.rootContext());
        window_ = qobject_cast<QQuickWindow*>(object);
        if (!window_) { delete object; return; }
        window_->QObject::setParent(this);
        window_->setTransientParent(nullptr);
        const auto screens = QGuiApplication::screens();
        QList<kgc::ui::ScreenInfo> info;
        for (auto* screen : screens) info.append({screen->name(), screen->availableGeometry()});
        const auto placement = kgc::ui::restorePlacement(settings::logWindowPlacement(), info, kDefaultSize);
        window_->setScreen(screens[placement.screenIndex]);
        window_->setGeometry(placement.geometry);
        emit windowChanged();
    }
    if (window_->visibility() == QWindow::Minimized) window_->showNormal();
    else window_->show();
    window_->raise();
    window_->requestActivate();
    KGC_LOG_INFO(kCategory, AnalyzeEvent::LogWindowOpened, "로그 창 열림");
}
void AppLogController::savePlacement()
{
    if (!window_ || window_->visibility() != QWindow::Windowed || !window_->screen()) return;
    const auto* screen = window_->screen();
    const auto relative = window_->geometry().translated(-screen->availableGeometry().topLeft());
    settings::setLogWindowPlacement(kgc::ui::savePlacement(screen->name(), relative));
}
}
