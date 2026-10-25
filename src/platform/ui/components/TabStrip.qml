// [0.0.2 T12 T17] specs/features/0.0.2-theme.md
/** 선택한 탭을 밑줄로 구분해요.
 * 입력: model, currentIndex와 activated(index)를 사용해요.
 * 출력: PillGroup의 선택과 키보드 동작을 물려받고 선택된 탭에 밑줄을 그려요.
 * 실패: 목록 끝의 이동 제한과 select의 유효 위치 조건은 PillGroup을 따라요.
 */
import QtQuick
import Kgc.Ui

PillGroup {
    id: tabs
    delegate: ActionButton {
        required property int index
        required property var modelData
        text: modelData
        kind: "ghost"
        textColor: tabs.currentIndex === index ? Theme.ink : Theme.inkMuted
        focusPolicy: Qt.NoFocus
        onClicked: { tabs.forceActiveFocus(); tabs.select(index) }
        background: Rectangle { color: Theme.surface; border.color: tabs.activeFocus && tabs.highlightedIndex === index ? Theme.focus : "transparent" }
        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: Theme.lineWidth * 2; color: tabs.currentIndex === index ? Theme.accent : Theme.line }
    }
}
