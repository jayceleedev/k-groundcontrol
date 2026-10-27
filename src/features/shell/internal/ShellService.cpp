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
    KGC_LOG_INFO(kCategory, ShellEvent::AppStarted, "App started: %1, language: %2, data folder: %3",
        QCoreApplication::applicationVersion(), translator.language(), kgc::app::paths::Paths::dataDir());
    menuCollapsed(settings::leftMenuCollapsed());
}
ShellService::~ShellService() { KGC_LOG_INFO(kCategory, ShellEvent::AppStopped, "App stopped"); }
void ShellService::windowShown(int width, int height)
{
    KGC_LOG_INFO(kCategory, ShellEvent::WindowShown, "Window shown: %1x%2", width, height);
}
void ShellService::panelOpened(const QString& id)
{
    KGC_LOG_INFO(kCategory, ShellEvent::PanelOpened, "Panel opened: %1", id);
}
void ShellService::panelClosed() { KGC_LOG_INFO(kCategory, ShellEvent::PanelClosed, "Panel closed"); }
void ShellService::menuCollapsed(bool collapsed)
{
    KGC_LOG_DEBUG(kCategory, ShellEvent::MenuCollapsed, "Left menu collapsed: %1", collapsed ? "true" : "false");
}
}
