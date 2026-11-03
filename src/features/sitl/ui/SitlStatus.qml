// [0.4.0 6.8] specs/features/0.4.0-sitl.md
/** 시뮬레이터 상태 카드와 시작·정지·다시 받기·다시 연결 단추, 실패·다른 연결 배너를 보여요.
 * 입력: SitlController의 상태, Job, 링크 여부, 캐시 시각이에요.
 * 출력: startOrStop·redownload·reconnect 요청이에요. 내려받는 중의 취소는 JobView가 Job에 요청해요.
 * 실패: Failed면 닫기 없는 경고 배너로 실패 글자를 보여요.
 * 흐름:
 * 1. SitlController.startOrStop — 시작하거나 정지해요.
 * 2. SitlController.redownload — 파일을 다시 받아요.
 * 3. SitlController.reconnect — 닫힌 UDP 링크를 다시 열어요.
 */
import QtQuick
import QtQuick.Layouts
import Kgc.Ui
import Kgc.Sitl

Column {
    id: root
    spacing: Theme.inset
    readonly property int sitlState: SitlController.state
    readonly property bool active: sitlState === Sitl.Starting || sitlState === Sitl.Running
    readonly property bool downloading: sitlState === Sitl.Downloading
    readonly property string failure: {
        const language = translator.language; const current = SitlController.state
        return SitlController.failureText()
    }
    // 닫기 단추 없이 상태가 바뀔 때까지 남는 배너예요.
    Banner {
        objectName: "sitlLinkBusy"
        width: root.width
        closable: false
        level: "info"
        visible: SitlController.linkBusy && SitlController.editable
        text: qsTr("다른 연결이 열려 있어요. 연결을 끊은 뒤 시작할 수 있어요.")
    }
    Banner {
        objectName: "sitlBanner"
        width: root.width
        closable: false
        level: "warn"
        visible: root.sitlState === Sitl.Failed
        text: root.failure
    }
    Rectangle {
        width: parent.width
        height: card.implicitHeight + Theme.panelInset * 2
        radius: Theme.radiusMedium
        color: Theme.surface
        border.color: Theme.line
        border.width: Theme.lineWidth
        RowLayout {
            id: card
            anchors.fill: parent
            anchors.margins: Theme.panelInset
            spacing: Theme.panelInset
            Chip {
                objectName: "sitlStateChip"
                Layout.alignment: Qt.AlignTop
                level: root.sitlState === Sitl.Running ? "ok" : root.sitlState === Sitl.Failed ? "danger"
                    : root.downloading || root.sitlState === Sitl.Starting ? "info" : "idle"
                text: root.sitlState === Sitl.Downloading ? qsTr("받는 중") : root.sitlState === Sitl.Starting ? qsTr("시작하는 중")
                    : root.sitlState === Sitl.Running ? qsTr("실행 중") : root.sitlState === Sitl.Failed ? qsTr("멈췄어요") : qsTr("멈춤")
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: Theme.valueLabelGap
                Text {
                    Layout.fillWidth: true
                    visible: root.sitlState === Sitl.Running
                    text: SitlController.linkOpenedBySitl ? qsTr("UDP 14550으로 연결했어요") : qsTr("연결이 끊겼어요")
                    color: Theme.ink
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSize
                }
                JobView {
                    objectName: "sitlJob"
                    Layout.fillWidth: true
                    visible: root.downloading && SitlController.job !== null
                    job: SitlController.job
                    showError: false
                }
                Text {
                    objectName: "sitlCacheText"
                    Layout.fillWidth: true
                    visible: !root.downloading
                    text: SitlController.cachedAt !== "" ? qsTr("받은 파일: %1").arg(SitlController.cachedAt)
                        : qsTr("처음 시작하면 파일을 내려받아요 (약 15 MB)")
                    color: Theme.inkMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSize
                    wrapMode: Text.Wrap
                }
            }
            ActionButton {
                objectName: "sitlReconnect"
                Layout.alignment: Qt.AlignTop
                visible: root.sitlState === Sitl.Running && !SitlController.linkOpenedBySitl
                kind: "secondary"
                text: qsTr("다시 연결")
                onClicked: SitlController.reconnect()
            }
            ActionButton {
                objectName: "sitlRedownload"
                Layout.alignment: Qt.AlignTop
                visible: !root.downloading
                enabled: SitlController.editable
                kind: "secondary"
                text: qsTr("다시 받기")
                onClicked: SitlController.redownload()
            }
            ActionButton {
                objectName: "sitlStart"
                Layout.alignment: Qt.AlignTop
                visible: !root.downloading
                enabled: root.active || SitlController.canStart
                kind: root.active ? "danger" : "primary"
                text: root.active ? qsTr("정지") : qsTr("시작")
                onClicked: SitlController.startOrStop()
            }
        }
    }
}
