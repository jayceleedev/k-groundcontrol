// [0.0.2 T12 T14] specs/features/0.0.2-theme.md
// [0.0.4 T6a] specs/features/0.0.4-righttabs.md
/** 값과 이름표와 오류를 한 입력란에 묶어요.
 * 입력: text, label, placeholderText, errorText, readOnly, echoMode와 editingFinished를 사용해요.
 * 출력: 편집한 text와 editingFinished를 제공하고 errorText를 오류 색으로 보여줘요.
 *       enabled가 거짓이면 ActionButton처럼 흐리게(바탕 surfaceAlt, 글자 inkDisabled, 테두리 line) 그려요.
 * 실패: 입력값을 스스로 판정하지 않아요. 호출자가 준 errorText를 표시해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.TextField {
    id: control
    property string label: ""
    property string errorText: ""
    implicitWidth: Theme.inputWidth
    implicitHeight: Theme.controlHeight + (label ? Theme.inputLabelHeight : 0) + (errorText ? Theme.inputLabelHeight : 0)
    topPadding: Theme.inset + (label ? Theme.inputLabelHeight : 0)
    bottomPadding: Theme.inset + (errorText ? Theme.inputLabelHeight : 0)
    padding: Theme.inset
    color: enabled ? Theme.ink : Theme.inkDisabled
    placeholderTextColor: enabled ? Theme.inkMuted : Theme.inkDisabled
    selectionColor: Theme.accent
    selectedTextColor: Theme.accentInk
    font.family: Theme.fontFamily
    font.pixelSize: Theme.fontSize
    selectByMouse: true
    background: Rectangle {
        y: control.label ? Theme.inputLabelHeight : 0
        height: control.height - y - (control.errorText ? Theme.inputLabelHeight : 0)
        color: control.enabled ? Theme.surface : Theme.surfaceAlt
        border.color: !control.enabled ? Theme.line : control.errorText ? Theme.danger : control.activeFocus ? Theme.focus : Theme.lineStrong
        border.width: control.activeFocus ? Theme.lineWidth * 2 : Theme.lineWidth
        radius: Theme.radiusSmall
    }
    Text { text: control.label; color: control.enabled ? Theme.inkMuted : Theme.inkDisabled; font: control.font; visible: text.length > 0 }
    Text { text: control.errorText; color: Theme.danger; font: control.font; anchors.bottom: parent.bottom; visible: text.length > 0 }
}
