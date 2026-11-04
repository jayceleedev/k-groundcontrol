// [0.4.1 6.4 T13] specs/features/0.4.1-board-match.md
// [0.4.0 6.8] specs/features/0.4.0-sitl.md
/** 시뮬레이션할 기체·배포판·홈·실행 값을 고르고 상태와 출력을 보여요.
 * 입력: SitlController의 상태와 입력값이에요.
 * 출력: 고르기와 시작·정지 요청이에요.
 * 실패: 실패와 다른 연결은 SitlStatus의 배너가 보여요.
 * 흐름:
 * 1. SitlController.startOrStop — 시작하거나 정지해요.
 * 2. SitlController.chooseVehicle·chooseRelease·resetHome — 입력값을 바꿔요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import Kgc.Sitl

Basic.ScrollView {
    id: panel
    objectName: "sitlPanel"
    property Item glassSource: null
    contentWidth: availableWidth
    rightPadding: Theme.scrollGutter
    Basic.ScrollBar.vertical.implicitWidth: Theme.scrollBarWidth
    clip: true
    /** 메뉴에서 열었을 때 초점을 받아요. 입력: 탐색 인자예요. 출력: 초점이에요. 실패: 없어요. */
    function onArg(arg: var) { forceActiveFocus() }
    component SectionLabel: Text {
        color: Theme.inkMuted
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        font.bold: true
    }
    Column {
        width: Math.min(panel.availableWidth, 960 * Theme.uiScale)
        // 패널 전체 폭의 가운데에 두되, 오른쪽 스크롤 여백 때문에 가로 스크롤이 생기지 않게 안쪽 폭에서 멈춰요.
        x: Math.max(0, Math.min((panel.width - width) / 2 - panel.leftPadding, panel.availableWidth - width))
        spacing: Theme.panelInset
        Column {
            width: parent.width
            spacing: Theme.valueLabelGap
            Text {
                text: qsTr("시뮬레이션")
                color: Theme.ink
                font.family: Theme.fontFamily
                font.pixelSize: Theme.titleSize
                font.bold: true
            }
            Text {
                width: parent.width
                text: qsTr("ArduPilot SITL을 내려받아 이 PC에서 실행해요.")
                color: Theme.inkMuted
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
                wrapMode: Text.Wrap
            }
        }
        SitlChoices { width: parent.width }
        SitlHomeFields { width: parent.width }
        Column {
            width: parent.width
            spacing: Theme.inset
            enabled: SitlController.editable
            SectionLabel { text: qsTr("실행") }
            Row {
                spacing: Theme.panelInset
                Column {
                    spacing: Theme.valueLabelGap
                    Text { text: qsTr("속도 배수"); color: Theme.inkMuted; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSize }
                    NumberBox {
                        id: speedupBox
                        objectName: "sitlSpeedup"
                        width: Theme.topbarFieldWidth
                        from: SitlController.speedupMin
                        to: SitlController.speedupMax
                        value: SitlController.speedup
                        // 고친 값을 컨트롤러에 넘긴 뒤 다시 컨트롤러 값에 묶어요.
                        onValueModified: next => {
                            SitlController.speedup = Math.round(next)
                            speedupBox.value = Qt.binding(() => SitlController.speedup)
                        }
                    }
                }
                Column {
                    anchors.bottom: parent.bottom
                    CheckRow {
                        objectName: "sitlWipe"
                        tristate: false
                        text: qsTr("저장된 파라미터를 지우고 시작")
                        checked: SitlController.wipe
                        onClicked: SitlController.wipe = checked
                    }
                    Text {
                        leftPadding: Theme.iconSize + Theme.inset * 2
                        text: qsTr("기본 파라미터로 처음부터 시작해요")
                        color: Theme.inkMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.captionSize
                    }
                }
            }
        }
        SitlStatus { width: parent.width }
        SitlOutput { width: parent.width }
    }
}
