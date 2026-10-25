// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 설명 풍선을 갖는 아이콘 단추예요.
 * 입력: name, hint, icon.source, clicked를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

ActionButton {
    id: control
    property string name: ""
    property string hint: ""
    kind: "secondary"
    icon.source: name ? "qrc:/kgc/icons/" + name + ".svg" : ""
    icon.color: enabled ? Theme.ink : Theme.inkDisabled
    implicitWidth: Theme.menuHeight
    Accessible.name: hint
    HintTip { text: control.hint; visible: control.hovered && text.length > 0 }
}
