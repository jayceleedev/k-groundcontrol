// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 선택 상태를 글자와 함께 보여줘요.
 * 입력: text, checked, enabled와 clicked를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.Switch {
    id: control
    implicitHeight: Theme.menuHeight
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
        width: Theme.menuHeight
        height: Theme.iconSize + Theme.menuGap * 2
        radius: height / 2
        color: !control.enabled ? Theme.line : control.checked ? Theme.accent : control.down ? Theme.line : control.hovered ? Theme.surfaceAlt : Theme.surface
        border.color: control.activeFocus ? Theme.focus : Theme.lineStrong
        border.width: control.activeFocus ? Theme.lineWidth * 2 : Theme.lineWidth
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            x: control.checked ? parent.width - width - Theme.menuGap : Theme.menuGap
            width: Theme.iconSize; height: width; radius: height / 2; color: Theme.accentInk
        }
    }
    contentItem: Text { text: control.text; font: control.font; color: control.enabled ? Theme.ink : Theme.inkDisabled; verticalAlignment: Text.AlignVCenter }
}
