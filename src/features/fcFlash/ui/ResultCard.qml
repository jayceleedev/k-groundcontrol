// [0.2.0 T30] specs/features/0.2.0-fcflash.md
/** 굽기의 결과를 가운데 정렬 카드로 보여요.
 * 입력: icon(icons.txt의 이름), iconColor, title, detail(비면 숨겨요)이에요.
 * 출력: 아이콘 아래에 제목과 내용을 그려요.
 * 실패: 없어요.
 */
import QtQuick
import Kgc.Ui

Rectangle {
    id: card
    property string icon: ""
    property color iconColor: Theme.ink
    property string title
    property string detail: ""
    implicitHeight: content.implicitHeight + Theme.panelInset * 2
    color: Theme.surface
    radius: Theme.radiusMedium
    border.width: Theme.lineWidth
    border.color: Theme.line
    Column {
        id: content
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: Theme.panelInset
        spacing: Theme.inset
        IconGlyph {
            anchors.horizontalCenter: parent.horizontalCenter
            width: Theme.iconSize * 2
            height: width
            name: card.icon
            color: card.iconColor
        }
        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            text: card.title
            color: Theme.ink
            font.family: Theme.fontFamily
            font.pixelSize: Theme.titleSize
            font.bold: true
        }
        Text {
            width: parent.width
            visible: text !== ""
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            text: card.detail
            color: Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
        }
    }
}
