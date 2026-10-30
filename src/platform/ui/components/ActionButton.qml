// [0.0.2 T12 T14 T17] specs/features/0.0.2-theme.md
// [0.0.4 T34] specs/features/0.0.4-righttabs.md
/** 동작의 중요도와 초점을 구분하는 단추예요.
 * 입력: text, icon, trailingIcon(글자 뒤 아이콘 이름, 기본 빈 값), kind, checked, enabled를 받아요.
 * textColor, radius, outlineColor, outlineWidth로 파생 부품의 강조를 정해요.
 * 출력: 눌렀을 때 clicked를 내고 textColor로 현재 글자 색을 제공해요.
 * 실패: enabled가 거짓이면 사용자 입력으로 clicked를 내지 않아요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.Button {
    id: control
    property string kind: "primary"
    property string trailingIcon: ""
    property color textColor: !enabled ? Theme.inkDisabled : kind === "primary" ? Theme.accentInk
        : kind === "danger" ? Theme.dangerInk : kind === "ghost" ? Theme.accent : Theme.ink
    property real radius: Theme.radiusSmall
    property color outlineColor: activeFocus ? Theme.focus : kind === "ghost" ? "transparent" : Theme.line
    property real outlineWidth: activeFocus ? Theme.lineWidth * 2 : Theme.lineWidth
    implicitHeight: Theme.controlHeight
    implicitWidth: Math.max(Theme.controlHeight, contentItem.implicitWidth + padding * 2)
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
        IconGlyph {
            visible: control.trailingIcon !== ""
            name: control.trailingIcon
            color: control.textColor
            width: visible ? control.icon.width : 0
            height: control.icon.height
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
