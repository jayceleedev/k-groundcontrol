// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 값과 이름표와 오류를 한 입력란에 묶어요.
 * 입력: text, label, placeholderText, errorText, readOnly, echoMode와 editingFinished를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.TextField {
    id: control
    property string label: ""
    property string errorText: ""
    implicitWidth: Theme.languageWidth
    implicitHeight: Theme.menuHeight + (label ? Theme.menuHeight : 0) + (errorText ? Theme.menuHeight : 0)
    topPadding: Theme.inset + (label ? Theme.menuHeight : 0)
    bottomPadding: Theme.inset + (errorText ? Theme.menuHeight : 0)
    padding: Theme.inset
    color: enabled ? Theme.ink : Theme.inkDisabled
    placeholderTextColor: Theme.inkMuted
    selectionColor: Theme.accent
    selectedTextColor: Theme.onAccent
    font.family: Theme.fontFamily
    font.pixelSize: Theme.fontSize
    selectByMouse: true
    background: Rectangle {
        y: control.label ? Theme.menuHeight : 0
        height: control.height - y - (control.errorText ? Theme.menuHeight : 0)
        color: Theme.surface
        border.color: control.errorText ? Theme.danger : control.activeFocus ? Theme.focus : Theme.lineStrong
        border.width: control.activeFocus ? Theme.lineWidth * 2 : Theme.lineWidth
        radius: Theme.radiusSmall
    }
    Text { text: control.label; color: Theme.inkMuted; font: control.font; visible: text.length > 0 }
    Text { text: control.errorText; color: Theme.danger; font: control.font; anchors.bottom: parent.bottom; visible: text.length > 0 }
}
