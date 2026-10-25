// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 선택한 탭을 밑줄로 구분해요.
 * 입력: model, currentIndex와 activated(index)를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

FocusScope {
    id: tabs
    property var model: []
    property int currentIndex: 0
    property int highlightedIndex: currentIndex
    signal activated(int index)
    activeFocusOnTab: true
    Keys.onLeftPressed: highlightedIndex = Math.max(0, highlightedIndex - 1)
    Keys.onRightPressed: highlightedIndex = Math.min(model.length - 1, highlightedIndex + 1)
    Keys.onSpacePressed: event => { if (!event.isAutoRepeat) select(highlightedIndex) }
    Keys.onReturnPressed: event => { if (!event.isAutoRepeat) select(highlightedIndex) }
    Keys.onEnterPressed: event => { if (!event.isAutoRepeat) select(highlightedIndex) }
    function select(index: int) { currentIndex = index; highlightedIndex = index; activated(index) }
    implicitWidth: row.implicitWidth
    implicitHeight: Theme.menuHeight
    Row {
        id: row
        anchors.fill: parent
        Repeater {
            model: tabs.model
            ActionButton {
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
    }
}
