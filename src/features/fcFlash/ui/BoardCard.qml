// [0.4.1 6.1] specs/features/0.4.1-board-match.md
// [0.2.0 T14] specs/features/0.2.0-fcflash.md
/** 부트로더에서 읽은 보드 정보를 카드 하나로 보여요.
 * 입력: 컨트롤러의 USB 장치 이름, USB 제조사·제품, 보드 ID, 플래시 크기, 부트로더 판이에요.
 * 출력: 아이콘, 보드 이름과 정보, 부트로더 상태 칩이에요. 제목은 USB 장치 이름이 있으면 그 값이에요.
 * 실패: USB 장치 이름이 없고 제조사나 제품을 모르면 "—"로 보여요.
 */
import QtQuick
import QtQuick.Layouts
import Kgc.Ui
import Kgc.FcFlash

Rectangle {
    implicitHeight: row.implicitHeight + Theme.panelInset * 2
    color: Theme.surface
    radius: Theme.radiusMedium
    border.width: Theme.lineWidth
    border.color: Theme.line
    RowLayout {
        id: row
        anchors.fill: parent
        anchors.margins: Theme.panelInset
        spacing: Theme.panelInset
        IconGlyph {
            Layout.preferredWidth: Theme.iconSize * 2
            Layout.preferredHeight: Theme.iconSize * 2
            name: "menu/fcFlash"
            color: Theme.inkMuted
        }
        Column {
            Layout.fillWidth: true
            spacing: Theme.valueLabelGap
            Text {
                width: parent.width
                wrapMode: Text.Wrap
                text: FcFlashController.usbName
                    || (FcFlashController.boardManufacturer || "—") + " " + (FcFlashController.boardProduct || "—")
                color: Theme.ink
                font.family: Theme.fontFamily
                font.pixelSize: Theme.titleSize
                font.bold: true
            }
            Text {
                width: parent.width
                wrapMode: Text.Wrap
                text: qsTr("보드 ID %1 · 플래시 %2 KB · 부트로더 %3")
                    .arg(FcFlashController.boardId).arg(FcFlashController.flashKb).arg(FcFlashController.bootloaderRevision)
                color: Theme.inkMuted
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
            }
        }
        Chip {
            text: qsTr("부트로더")
            level: "ok"
        }
    }
}
