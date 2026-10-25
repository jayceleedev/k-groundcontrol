// [0.0.3 T3 T4 T5 T12 T13] specs/features/0.0.3-windows.md
#include "AppLogController.h"
#include <kgc/analyze/AppLogService.h>
#include <kgc/analyze/Settings.h>
#include "Constants.h"
#include <kgc/analyze/ModuleId.h>
#include <kgc/app/log/Log.h>
#include "Events.h"
namespace kgc::analyze {
AppLogController::AppLogController(AppLogService& service, QQmlEngine& engine)
    : QObject(&service), service_(service),
      window_(engine, QUrl("qrc:/qt/qml/Kgc/Analyze/AppLogWindow.qml"), kDefaultSize,
          [] { return settings::logWindowPlacement(); },
          [](const QString& value) { settings::setLogWindowPlacement(value); }, this)
{
    connect(&window_, &kgc::ui::SeparateWindow::windowChanged, this, &AppLogController::windowChanged);
}
QQuickWindow* AppLogController::window() const { return window_.window(); }
int AppLogController::saveDelayMs() const { return kSaveDelayMs; }
QAbstractListModel* AppLogController::model() const { return service_.model(); }
void AppLogController::open()
{
    if (window_.open()) KGC_LOG_INFO(kCategory, AnalyzeEvent::LogWindowOpened, "로그 창 열림");
}
void AppLogController::savePlacement() { window_.savePlacement(); }
}
