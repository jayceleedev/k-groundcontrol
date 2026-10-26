// [0.0.2 T12 T17] specs/features/0.0.2-theme.md
/** 탭 정지 하나로 선택 목록을 다룰 수 있어요.
 * 입력: model은 글자 목록이고 currentIndex, highlightedIndex, select(index)를 사용해요. delegate로 선택 단추 모양을 바꿀 수 있어요.
 *       spacing은 선택 단추 사이 간격이고 기본값은 Theme.choiceGap이에요.
 * 출력: 한 번의 Tab 정지에서 highlightedIndex로 이동하고 select(index)가 currentIndex와 activated(index)를 갱신해요.
 * 실패: 화살표 이동은 목록 양 끝에서 멈춰요. select에는 목록 안 위치를 넘겨야 해요.
 */
import QtQuick
import Kgc.Ui

FocusScope {
    id: group
    property var model: []
    property int currentIndex: 0
    property int highlightedIndex: currentIndex
    property real spacing: Theme.choiceGap
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
    property Component delegate: Pill {
        required property int index
        required property var modelData
        text: modelData
        checked: group.currentIndex === index
        focusPolicy: Qt.NoFocus
        onClicked: { group.forceActiveFocus(); group.select(index) }
        outlineColor: group.activeFocus && group.highlightedIndex === index ? Theme.focus : Theme.line
        outlineWidth: group.activeFocus && group.highlightedIndex === index ? Theme.lineWidth * 2 : Theme.lineWidth
    }
    Row {
        id: row
        spacing: group.spacing
        Repeater {
            model: group.model
            delegate: group.delegate
        }
    }
}
