// [0.0.2 T12 T14 T23] specs/features/0.0.2-theme.md
/** 설명 풍선을 갖는 아이콘 단추예요.
 * 입력: name, hint, icon.source, clicked를 사용해요.
 * 출력: 이름에 맞는 아이콘과 설명 풍선을 그리고 clicked를 내요.
 * 실패: 빈 hint에는 풍선을 띄우지 않고, 없는 아이콘은 그리지 못해요.
 */
import QtQuick
import Kgc.Ui

ActionButton {
    id: control
    property string name: ""
    property string hint: ""
    kind: "secondary"
    icon.source: name ? "qrc:/kgc/icons/" + name + ".svg" : ""
    icon.color: enabled ? Theme.ink : Theme.inkDisabled
    implicitWidth: Theme.controlHeight
    Accessible.name: hint
    HintTip { text: control.hint; visible: control.hovered && text.length > 0 }
}
