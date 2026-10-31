// [0.2.0 T14] specs/features/0.2.0-fcflash.md
#include <kgc/fcFlash/Ui.h>
#include <kgc/ui/UiRegistry.h>
#include "FcFlashController.h"
namespace kgc::fcFlash {
void registerUi(kgc::ui::UiRegistry& registry, FcFlashService& service)
{
    // WORKER-FILL: spec §6.9 — PanelRegistry에 id fcFlash, 순서 1, 그룹 1, 제목 "펌웨어 세팅", 패널 FcFlashPanel.qml을 등록해요.
    Q_UNUSED(registry);
    new FcFlashController(service);
}
}
