// [0.0.2 T12 T17] specs/features/0.0.2-theme.md
// [0.0.4 T4 T10a T10b] specs/features/0.0.4-righttabs.md
/** 선택한 탭을 밑줄로 구분해요.
 * 입력: model, currentIndex와 activated(index)를 사용해요. fill이 참이면 줄 폭을 탭 수로 똑같이 나눠 채우고
 *       (단추 높이 = 줄 높이, 가로 안쪽 여백 Theme.inset / 2), 칸보다 긴 이름은 끝을 …로 줄여요.
 *       buttonNames는 탭 단추마다 붙일 objectName 목록이에요(비면 이름 없음).
 * 출력: PillGroup의 선택과 키보드 동작을 물려받고 선택된 탭에 밑줄을 그려요. 이름 글자의 objectName은 tabLabel이고,
 *       줄인 탭에 마우스를 올리면 전체 이름을 HintTip으로 보여요. fill이 거짓이면 이름 길이만큼의 단추를 나란히 둬요.
 * 실패: 목록 끝의 이동 제한과 select의 유효 위치 조건은 PillGroup을 따라요. fill이면 부르는 쪽이 줄 폭을 정해야 해요.
 */
import QtQuick
import Kgc.Ui

PillGroup {
    id: tabs
    property bool fill: false
    property var buttonNames: []
    delegate: ActionButton {
        id: tab
        required property int index
        required property var modelData
        objectName: tabs.buttonNames[index] ?? ""
        text: modelData
        kind: "ghost"
        width: tabs.fill ? tabs.width / Math.max(1, tabs.model.length) : implicitWidth
        height: tabs.fill ? tabs.height : implicitHeight
        leftPadding: tabs.fill ? Theme.inset / 2 : Theme.inset
        rightPadding: tabs.fill ? Theme.inset / 2 : Theme.inset
        textColor: tabs.currentIndex === index ? Theme.ink : Theme.inkMuted
        focusPolicy: Qt.NoFocus
        onClicked: { tabs.forceActiveFocus(); tabs.select(index) }
        contentItem: Text {
            objectName: "tabLabel"
            text: tab.text
            font: tab.font
            color: tab.textColor
            elide: Text.ElideRight
            horizontalAlignment: tabs.fill ? Text.AlignHCenter : Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle { color: Theme.surface; border.color: tabs.activeFocus && tabs.highlightedIndex === index ? Theme.focus : "transparent" }
        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: Theme.lineWidth * 2; color: tabs.currentIndex === index ? Theme.accent : Theme.line }
        HintTip { text: tab.text; visible: tab.hovered && tab.contentItem.truncated }
        // PillGroup의 단추 사이 간격이 남아 있으면 똑같이 나눈 탭의 합이 줄 폭을 넘어서, 나눠 채울 때만 단추를 담은 줄(부모 Row)의 간격을 0으로 해요.
        Binding { when: tabs.fill; target: tab.parent; property: "spacing"; value: 0 }
    }
}
