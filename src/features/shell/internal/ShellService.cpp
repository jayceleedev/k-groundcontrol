// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#include <kgc/shell/ShellService.h>
#include <kgc/shell/ModuleId.h>
#include <kgc/shell/Settings.h>
#include <kgc/app/i18n/Translator.h>
#include <kgc/app/paths/Paths.h>
#include <kgc/app/log/Log.h>
#include <QCoreApplication>
#include "Events.h"
namespace kgc::shell {
ShellService::ShellService(kgc::app::i18n::Translator& translator, QObject* parent) : QObject(parent)
{
    KGC_LOG_INFO(kCategory, ShellEvent::AppStarted, "앱 시작: %1, 언어: %2, 데이터 폴더: %3",
        QCoreApplication::applicationVersion(), translator.language(), kgc::app::paths::Paths::dataDir());
    menuCollapsed(settings::leftMenuCollapsed());
}
ShellService::~ShellService() { KGC_LOG_INFO(kCategory, ShellEvent::AppStopped, "앱 종료"); }
void ShellService::windowShown(int width, int height)
{
    KGC_LOG_INFO(kCategory, ShellEvent::WindowShown, "창 표시: %1x%2", width, height);
}
void ShellService::panelOpened(const QString& id)
{
    KGC_LOG_INFO(kCategory, ShellEvent::PanelOpened, "패널 열림: %1", id);
}
void ShellService::panelClosed() { KGC_LOG_INFO(kCategory, ShellEvent::PanelClosed, "패널 닫힘"); }
void ShellService::menuCollapsed(bool collapsed)
{
    KGC_LOG_DEBUG(kCategory, ShellEvent::MenuCollapsed, "좌 메뉴 접힘: %1", collapsed ? "true" : "false");
}
}
