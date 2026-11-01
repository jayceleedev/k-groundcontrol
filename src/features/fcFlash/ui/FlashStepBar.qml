// [0.2.0 T26] specs/features/0.2.0-fcflash.md
/** 굽기 네 단계를 번호 원과 이어진 선으로 보여요.
 * 입력: currentStep은 지금 단계의 순서예요(0부터 3까지).
 * 출력: 끝난 단계, 현재 단계, 뒤 단계를 서로 다른 모양으로 그려요.
 * 실패: 없어요. 범위 밖 값이면 그 값보다 앞선 단계를 끝난 단계로 그려요.
 */
import QtQuick
import QtQuick.Layouts
import Kgc.Ui

RowLayout {
    id: root
    property int currentStep: 0
    spacing: Theme.inset
    Repeater {
        id: steps
        model: [qsTr("보드 확인"), qsTr("펌웨어 선택"), qsTr("확인"), qsTr("굽기")]
        RowLayout {
            id: step
            required property string modelData
            required property int index
            readonly property bool finished: index < root.currentStep
            readonly property bool current: index === root.currentStep
            // 마지막 단계에는 이을 선이 없어서 남는 폭을 앞 단계의 선들이 나눠 가져요.
            Layout.fillWidth: index < steps.count - 1
            spacing: Theme.inset
            Rectangle {
                Layout.preferredWidth: Theme.flatIconButtonSize
                Layout.preferredHeight: Theme.flatIconButtonSize
                radius: width / 2
                color: step.finished ? Theme.okSoft : step.current ? Theme.accent : "transparent"
                border.width: Theme.lineWidth
                border.color: step.finished ? Theme.okSoft : step.current ? Theme.accent : Theme.line
                IconGlyph {
                    anchors.centerIn: parent
                    width: Theme.flatIconSize
                    height: Theme.flatIconSize
                    visible: step.finished
                    name: "check"
                    color: Theme.ok
                }
                Text {
                    anchors.centerIn: parent
                    visible: !step.finished
                    text: step.index + 1
                    color: step.current ? Theme.accentInk : Theme.inkMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSize
                }
            }
            Text {
                text: step.modelData
                color: step.current ? Theme.ink : Theme.inkMuted
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
                font.bold: step.current
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: Theme.lineWidth
                visible: step.index < steps.count - 1
                color: Theme.line
            }
        }
    }
}
