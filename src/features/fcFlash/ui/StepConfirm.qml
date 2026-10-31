// [0.2.0 T19 T22 T23] specs/features/0.2.0-fcflash.md
/** 보드와 올릴 펌웨어를 확인하고 길게 눌러 굽기를 시작해요.
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
    spacing: Theme.panelInset
    GridLayout {
        width: parent.width
        columns: 2
        columnSpacing: Theme.panelInset
        rowSpacing: Theme.inset
        Repeater {
            model: [
                qsTr("FC 이름"), (FcFlashController.boardManufacturer || "—") + " " + (FcFlashController.boardProduct || "—"),
                qsTr("올릴 펌웨어"), [FcFlashController.firmwareName, FcFlashController.expectedVersion].filter(value => value !== "").join(" ")
                    + (FcFlashController.firmwareBoard ? " · " + FcFlashController.firmwareBoard : ""),
                qsTr("출처"), FcFlashController.source === FlashStates.Px4Official ? qsTr("PX4 공식")
                    : FcFlashController.source === FlashStates.ArduPilotOfficial ? qsTr("ArduPilot 공식") : qsTr("내 PC 파일")
            ]
            Text {
                required property string modelData
                required property int index
                Layout.fillWidth: index % 2 === 1
                Layout.preferredWidth: index % 2 === 0 ? Theme.inputWidth / 2 : -1
                Layout.minimumHeight: Theme.controlHeight
                text: modelData
                color: index % 2 === 0 ? Theme.inkMuted : Theme.ink
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
                wrapMode: Text.Wrap
                verticalAlignment: Text.AlignVCenter
            }
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
