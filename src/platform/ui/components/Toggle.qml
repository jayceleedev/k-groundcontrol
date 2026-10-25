// [0.0.2 T12 T17] specs/features/0.0.2-theme.md
/** 선택 상태를 글자와 함께 보여줘요.
 * 입력: text, checked, enabled와 clicked를 사용해요.
 * 출력: checked 상태를 손잡이 위치로 보여 주고 clicked와 toggled를 내요.
 * 실패: 비활성 토글은 사용자 조작으로 상태를 바꾸지 않아요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.Switch {
    id: control
    implicitHeight: Theme.controlHeight
    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    padding: Theme.inset
    leftPadding: indicator.width + Theme.inset * 2
    font.family: Theme.fontFamily
    font.pixelSize: Theme.fontSize
    hoverEnabled: true
    Keys.onReturnPressed: event => { if (!event.isAutoRepeat) control.click() }
    Keys.onEnterPressed: event => { if (!event.isAutoRepeat) control.click() }
    indicator: Rectangle {
        x: Theme.inset
        y: (control.height - height) / 2
        width: Theme.controlHeight
        height: Theme.iconSize + Theme.switchInset * 2
        radius: height / 2
        color: !control.enabled ? Theme.line : control.checked ? Theme.accent : control.down ? Theme.line : control.hovered ? Theme.surfaceAlt : Theme.surface
        border.color: control.activeFocus ? Theme.focus : Theme.lineStrong
        border.width: control.activeFocus ? Theme.lineWidth * 2 : Theme.lineWidth
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            x: control.checked ? parent.width - width - Theme.switchInset : Theme.switchInset
            width: Theme.iconSize; height: width; radius: height / 2; color: Theme.accentInk
        }
    }
    contentItem: Text { text: control.text; font: control.font; color: control.enabled ? Theme.ink : Theme.inkDisabled; verticalAlignment: Text.AlignVCenter }
}
