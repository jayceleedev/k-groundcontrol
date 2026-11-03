// [0.4.0 6.8] specs/features/0.4.0-sitl.md
/** 시뮬레이터 출력의 마지막 줄들을 고정폭 글꼴의 읽기 전용 상자로 보여요.
 * 입력: SitlController.outputText예요.
 * 출력: 새 줄이 오면 맨 아래로 내려가요.
 * 실패: 비어 있으면 안내 글을 보여요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import Kgc.Sitl

Column {
    id: root
    spacing: Theme.inset
    FontMetrics { id: metrics; font.family: Theme.monoFamily; font.pixelSize: Theme.fontSize }
    Text {
        text: qsTr("시뮬레이터 출력")
        color: Theme.inkMuted
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        font.bold: true
    }
    Basic.ScrollView {
        width: parent.width
        // 약 10줄 높이예요.
        height: Math.ceil(metrics.lineSpacing * 10) + Theme.inset * 2
        clip: true
        Basic.TextArea {
            id: area
            objectName: "sitlOutput"
            readOnly: true
            text: SitlController.outputText
            placeholderText: qsTr("시작하면 여기에 시뮬레이터 출력이 보여요.")
            placeholderTextColor: Theme.inkMuted
            color: Theme.ink
            font.family: Theme.monoFamily
            font.pixelSize: Theme.fontSize
            wrapMode: TextEdit.NoWrap
            padding: Theme.inset
            background: Rectangle {
                color: Theme.surfaceAlt
                radius: Theme.radiusMedium
                border.color: Theme.line
                border.width: Theme.lineWidth
            }
            onTextChanged: area.cursorPosition = area.length
        }
    }
}
