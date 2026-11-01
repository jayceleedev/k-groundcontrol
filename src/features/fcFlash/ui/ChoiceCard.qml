// [0.2.0 T27] specs/features/0.2.0-fcflash.md
/** 아이콘과 글자로 하나를 고르는 카드예요.
 * 입력: icon.name(icons.txt의 이름), title, subtitle(비면 숨겨요), selected, compact, enabled를 받아요.
 *       compact가 참이면 작은 아이콘과 subtitle만 쓰는 작은 카드가 돼요.
 * 출력: 마우스와 Tab+Space로 고르면 clicked를 내요. 선택 상태는 호출자가 selected로 정해요.
 * 실패: enabled가 거짓이면 사용자 입력으로 clicked를 내지 않아요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.AbstractButton {
    id: control
    property string title
    property string subtitle: ""
    property bool selected: false
    property bool compact: false
    readonly property color iconColor: !enabled ? Theme.inkDisabled : selected ? Theme.info : Theme.inkMuted
    readonly property color titleColor: !enabled ? Theme.inkDisabled : Theme.ink
    padding: compact ? Theme.inset : Theme.panelInset
    hoverEnabled: true
    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: implicitContentHeight + topPadding + bottomPadding
    contentItem: Column {
        spacing: control.compact ? Theme.valueLabelGap : Theme.inset
        IconGlyph {
            anchors.horizontalCenter: parent.horizontalCenter
            width: control.compact ? Theme.iconSize : Theme.iconSize * 2
            height: width
            name: control.icon.name
            color: control.iconColor
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            visible: !control.compact
            text: control.title
            color: control.titleColor
            font.family: Theme.fontFamily
            font.pixelSize: Theme.titleSize
            font.bold: true
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            visible: text !== ""
            text: control.subtitle
            color: control.compact || control.selected || !control.enabled ? control.titleColor : Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
        }
    }
    background: Rectangle {
        radius: Theme.radiusMedium
        color: !control.enabled ? Theme.surfaceAlt : control.selected ? Theme.infoSoft : control.hovered ? Theme.surfaceAlt : Theme.surface
        border.width: control.activeFocus || control.selected ? Theme.lineWidth * 2 : Theme.lineWidth
        border.color: control.activeFocus ? Theme.focus : control.selected ? Theme.accent : Theme.line
    }
}
