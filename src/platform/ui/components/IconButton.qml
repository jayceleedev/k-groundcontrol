// [0.0.2 T12 T14 T23] specs/features/0.0.2-theme.md
/** 설명 풍선을 갖는 아이콘 단추예요.
 * 입력: name, hint, icon.source, clicked를 사용해요. Button의 flat이 참이면 테두리·바탕 없이 아이콘만 그리고
 *       마우스를 올렸을 때만 옅은 바탕을 보여요(알림 줄·패널 머리·입력칸 안의 닫기·지우기용).
 * 출력: 이름에 맞는 아이콘과 설명 풍선을 그리고 clicked를 내요.
 * 실패: 빈 hint에는 풍선을 띄우지 않고, 없는 아이콘은 그리지 못해요.
 */
import QtQuick
import Kgc.Ui

ActionButton {
    id: control
    property string name: ""
    property string hint: ""
    kind: flat ? "ghost" : "secondary"
    icon.source: name ? "qrc:/kgc/icons/" + name + ".svg" : ""
    icon.color: enabled ? Theme.ink : Theme.inkDisabled
    icon.width: flat ? Theme.flatIconSize : Theme.iconSize
    icon.height: flat ? Theme.flatIconSize : Theme.iconSize
    padding: flat ? 0 : Theme.inset
    implicitWidth: flat ? Theme.flatIconButtonSize : Theme.controlHeight
    implicitHeight: flat ? Theme.flatIconButtonSize : Theme.controlHeight
    // 글자 없는 단추라서 빈 글자 칸이 높이·간격을 차지해 아이콘이 왼쪽 위로 밀리지 않게, 아이콘만 상자 한가운데에 놓아요.
    contentItem: Item {
        implicitWidth: control.icon.width
        implicitHeight: control.icon.height
        IconGlyph {
            anchors.centerIn: parent
            source: control.icon.source
            color: control.icon.color
            width: control.icon.width
            height: control.icon.height
        }
    }
    Accessible.name: hint
    HintTip { text: control.hint; visible: control.hovered && text.length > 0 }
}
