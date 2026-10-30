// [0.0.4 T41 T42 T43] specs/features/0.0.4-righttabs.md
/** 소개와 주소를 카드 폭에 맞춰 기본 브라우저 바로가기와 함께 보여요.
 * 입력: siteId, title, description, address와 유리 바탕 source.
 * 출력: 사이트 소개와 바로가기 단추가 있는 카드.
 * 실패: 브라우저를 열지 못했을 때 별도 안내는 하지 않아요.
 * 흐름:
 * 1. ActionButton.onClicked → Qt.openUrlExternally — 카드의 주소를 기본 브라우저에 넘겨요.
 */
import QtQuick
import Kgc.Ui

Card {
    id: card
    required property string siteId
    required property string description
    required property string address
    objectName: "referenceCard_" + siteId
    Text {
        width: parent.width
        text: card.description
        wrapMode: Text.Wrap
        color: Theme.ink
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
    }
    Item {
        width: parent.width
        implicitHeight: Math.max(addressText.implicitHeight, openButton.implicitHeight)
        Text {
            id: addressText
            anchors.left: parent.left
            anchors.right: openButton.left
            anchors.rightMargin: Theme.inset
            anchors.verticalCenter: parent.verticalCenter
            text: card.address
            wrapMode: Text.WrapAnywhere
            color: Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.captionSize
        }
        ActionButton {
            id: openButton
            objectName: "referenceOpen_" + card.siteId
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            kind: "secondary"
            text: qsTr("바로가기")
            icon.source: "qrc:/kgc/icons/external-link.svg"
            onClicked: Qt.openUrlExternally(card.address)
        }
    }
}
