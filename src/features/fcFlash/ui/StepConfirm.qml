// [0.2.0 T19 T22 T23 T28] specs/features/0.2.0-fcflash.md
/** 보드와 올릴 펌웨어를 두 카드로 확인하고 길게 눌러 굽기를 시작해요.
 * 입력: 보드 이름과 선택한 펌웨어예요.
 * 출력: 뒤로 또는 굽기 요청이에요.
 * 실패: 서비스의 시작 조건을 따라요.
 * 흐름:
 * 1. FcFlashController.backToFirmware·startFlash — 선택으로 돌아가거나 굽기를 시작해요.
 */
import QtQuick
import QtQuick.Layouts
import Kgc.Ui
import Kgc.FcFlash

Column {
    id: root
    spacing: Theme.panelInset
    // 화살표와 양쪽 간격을 뺀 폭을 두 카드가 똑같이 나눠요.
    readonly property real cardWidth: (width - Theme.iconSize - Theme.inset * 2) / 2
    /** 이름, 값, 덧붙임 글자를 묶은 확인 카드예요. 입력: caption, value, note, outline이에요. 출력: 카드예요. 실패: 없어요. */
    component ConfirmCard: Rectangle {
        id: card
        property string caption: ""
        property string value: ""
        property string note: ""
        property color outline: Theme.line
        implicitHeight: lines.implicitHeight + Theme.panelInset * 2
        color: Theme.surface
        radius: Theme.radiusMedium
        border.width: Theme.lineWidth
        border.color: card.outline
        Column {
            id: lines
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: Theme.panelInset
            spacing: Theme.valueLabelGap
            Text {
                width: parent.width
                text: card.caption
                color: Theme.inkMuted
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
            }
            Text {
                width: parent.width
                wrapMode: Text.Wrap
                text: card.value
                color: Theme.ink
                font.family: Theme.fontFamily
                font.pixelSize: Theme.titleSize
                font.bold: true
            }
            Text {
                width: parent.width
                visible: text !== ""
                wrapMode: Text.Wrap
                text: card.note
                color: Theme.inkMuted
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
            }
        }
    }
    RowLayout {
        width: parent.width
        spacing: Theme.inset
        ConfirmCard {
            Layout.preferredWidth: root.cardWidth
            Layout.fillHeight: true
            caption: qsTr("FC")
            value: (FcFlashController.boardManufacturer || "—") + " " + (FcFlashController.boardProduct || "—")
        }
        IconGlyph {
            Layout.alignment: Qt.AlignVCenter
            name: "arrow-right"
            color: Theme.inkMuted
        }
        ConfirmCard {
            Layout.preferredWidth: root.cardWidth
            Layout.fillHeight: true
            caption: qsTr("올릴 펌웨어")
            value: [FcFlashController.firmwareName, FcFlashController.expectedVersion].filter(text => text !== "").join(" ")
                + (FcFlashController.firmwareBoard ? " · " + FcFlashController.firmwareBoard : "")
            note: FcFlashController.source === FlashStates.Px4Official ? qsTr("PX4 공식")
                : FcFlashController.source === FlashStates.ArduPilotOfficial ? qsTr("ArduPilot 공식") : qsTr("내 PC 파일")
            outline: Theme.accent
        }
    }
    RowLayout {
        width: parent.width
        spacing: Theme.inset
        ActionButton { text: qsTr("뒤로"); kind: "secondary"; onClicked: FcFlashController.backToFirmware() }
        Item { Layout.fillWidth: true }
        HoldToConfirm {
            objectName: "fcFlashStart"
            text: qsTr("길게 눌러 굽기")
            enabled: FcFlashController.canContinue
            onConfirmed: FcFlashController.startFlash()
        }
    }
}
