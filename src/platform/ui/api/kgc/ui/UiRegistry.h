// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <kgc/ui/PanelRegistry.h>
#include <kgc/ui/Slots.h>
#include <kgc/ui/Navigator.h>
#include <QQmlEngine>

namespace kgc::ui {
/** 기능이 화면에 등록할 통로를 참조로 묶어요.
 * 입력: 등록부와 슬롯과 탐색기와 엔진.
 * 출력: 참조 묶음.
 * 실패: 없음.
 */
struct UiRegistry {
    PanelRegistry& panels;
    TopbarSlot& topbar;
    StatusChipSlot& statusChips;
    RightTabSlot& rightTabs;
    MapLayerSlot& mapLayers;
    BottomActionSlot& bottomActions;
    Navigator& navigator;
    QQmlEngine& engine;
};
}
