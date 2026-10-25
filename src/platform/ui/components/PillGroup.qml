// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 탭 정지 하나로 선택 목록을 다룰 수 있어요.
 * 입력: model은 글자 목록이고 currentIndex, highlightedIndex, activated(index)를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

FocusScope {
    id: group
    property var model: []
    property int currentIndex: 0
    property int highlightedIndex: currentIndex
    signal activated(int index)
    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight
    activeFocusOnTab: true
    Keys.onLeftPressed: highlightedIndex = Math.max(0, highlightedIndex - 1)
    Keys.onRightPressed: highlightedIndex = Math.min(model.length - 1, highlightedIndex + 1)
    Keys.onSpacePressed: event => { if (!event.isAutoRepeat) select(highlightedIndex) }
    Keys.onReturnPressed: event => { if (!event.isAutoRepeat) select(highlightedIndex) }
    Keys.onEnterPressed: event => { if (!event.isAutoRepeat) select(highlightedIndex) }
    function select(index: int) { currentIndex = index; highlightedIndex = index; activated(index) }
    Row {
        id: row
        spacing: Theme.menuGap
        Repeater {
            model: group.model
            Pill {
                required property int index
                required property var modelData
                text: modelData
                checked: group.currentIndex === index
                focusPolicy: Qt.NoFocus
                onClicked: { group.forceActiveFocus(); group.select(index) }
                outlineColor: group.activeFocus && group.highlightedIndex === index ? Theme.focus : Theme.line
                outlineWidth: group.activeFocus && group.highlightedIndex === index ? Theme.lineWidth * 2 : Theme.lineWidth
            }
        }
    }
}
