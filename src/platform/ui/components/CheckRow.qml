// [0.0.2 T12 T17] specs/features/0.0.2-theme.md
/** 선택 상태를 글자와 함께 보여줘요.
 * 입력: text, checked, enabled와 clicked를 사용해요. checkState로 세 상태를 구분해요.
 * 출력: checkState로 세 상태를 보여 주고 clicked와 checkStateChanged를 내요.
 * 실패: 비활성 체크 행은 눌러도 checkState를 바꾸지 않아요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.CheckBox {
    id: control
    tristate: true
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
        width: Theme.iconSize
        height: Theme.iconSize + 0
        radius: Theme.radiusSmall
        color: !control.enabled ? Theme.line : control.checked || control.checkState === Qt.PartiallyChecked ? Theme.accent : control.down ? Theme.line : control.hovered ? Theme.surfaceAlt : Theme.surface
        border.color: control.activeFocus ? Theme.focus : Theme.lineStrong
        border.width: control.activeFocus ? Theme.lineWidth * 2 : Theme.lineWidth
        Text { anchors.centerIn: parent; text: control.checkState === Qt.PartiallyChecked ? qsTr("−") : control.checked ? qsTr("✓") : ""; color: Theme.accentInk }
    }
    contentItem: Text { text: control.text; font: control.font; color: control.enabled ? Theme.ink : Theme.inkDisabled; verticalAlignment: Text.AlignVCenter }
}
