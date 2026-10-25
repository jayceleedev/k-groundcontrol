// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 동작의 중요도와 초점을 구분하는 단추예요.
 * 입력: text, icon, kind, checked, enabled를 받고 clicked를 내요.
 * textColor, radius, outlineColor, outlineWidth로 파생 부품의 강조를 정해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.Button {
    id: control
    property string kind: "primary"
    property color textColor: !enabled ? Theme.inkDisabled : kind === "primary" ? Theme.onAccent
        : kind === "danger" ? Theme.onDanger : kind === "ghost" ? Theme.accent : Theme.ink
    property real radius: Theme.radiusSmall
    property color outlineColor: activeFocus ? Theme.focus : kind === "ghost" ? "transparent" : Theme.line
    property real outlineWidth: activeFocus ? Theme.lineWidth * 2 : Theme.lineWidth
    implicitHeight: Theme.menuHeight
    implicitWidth: Math.max(Theme.menuHeight, contentItem.implicitWidth + padding * 2)
    padding: Theme.inset
    spacing: Theme.inset
    font.family: Theme.fontFamily
    font.pixelSize: Theme.fontSize
    hoverEnabled: true
    icon.width: Theme.iconSize
    icon.height: Theme.iconSize
    icon.color: textColor
    Keys.onReturnPressed: event => { if (!event.isAutoRepeat) control.click() }
    Keys.onEnterPressed: event => { if (!event.isAutoRepeat) control.click() }
    contentItem: Row {
        spacing: control.spacing
        IconGlyph {
            visible: control.icon.source.toString() !== ""
            source: control.icon.source
            color: control.icon.color
            width: visible ? control.icon.width : 0
            height: control.icon.height
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            text: control.text
            font: control.font
            color: control.textColor
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    background: Rectangle {
        radius: control.radius
        color: !control.enabled ? Theme.surfaceAlt : control.kind === "danger" ? Theme.danger
            : control.kind === "primary" ? (control.down ? Theme.accentPressed : control.hovered ? Theme.accentHover : Theme.accent)
            : control.down ? Theme.line : control.hovered ? Theme.surfaceAlt : control.kind === "ghost" ? "transparent" : Theme.surface
        border.width: control.outlineWidth
        border.color: control.outlineColor
    }
}
