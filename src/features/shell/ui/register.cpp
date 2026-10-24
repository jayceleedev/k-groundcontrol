// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#include <kgc/shell/Ui.h>
#include <kgc/shell/ShellService.h>
#include <kgc/ui/UiRegistry.h>
#include "ShellController.h"
#include <QQmlContext>
namespace kgc::shell {
void registerUi(kgc::ui::UiRegistry& registry, ShellService& service)
{
    registry.engine.rootContext()->setContextProperty("shellController", new ShellController(service, registry.navigator));
}
}
