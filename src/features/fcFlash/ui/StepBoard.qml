// [0.2.0 T14–T18] specs/features/0.2.0-fcflash.md
/** 포트 선택과 부트로더 탐색 상태를 보여요.
 * 입력: 서비스의 포트 목록과 탐색 상태예요.
 * 출력: 보드 확인과 후보 선택 요청이에요.
 * 실패: 현재 언어의 실패 문구와 다시 하기 단추를 보여요.
 * 흐름:
 * 1. FcFlashController.beginProbe·pickPort·retryProbe — 포트 확인과 선택을 서비스에 맡겨요.
 */
import QtQuick
import QtQuick.Layouts
import Kgc.Ui
import Kgc.FcFlash

Column {
    id: root
    spacing: Theme.panelInset
    property string selectedPort: ""
    readonly property int phase: FcFlashController.phase
    readonly property string failure: { const language = translator.language; const phase = FcFlashController.phase; return FcFlashController.failureText() }
    RowLayout {
        width: parent.width
        spacing: Theme.inset
        Text {
            text: qsTr("포트")
            color: Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
        }
        ComboField {
            id: port
            objectName: "fcFlashPort"
            model: FcFlashController.ports
            textRole: "label"
            valueRole: "name"
            currentIndex: root.selectedPort ? indexOfValue(root.selectedPort) : -1
            // 목록 교체 뒤에도 이름으로 선택을 복원하고, 사라진 포트는 선택을 비워요.
            onModelChanged: Qt.callLater(function() {
                if (port.indexOfValue(root.selectedPort) < 0) root.selectedPort = ""
                port.currentIndex = Qt.binding(function() {
                    return root.selectedPort ? port.indexOfValue(root.selectedPort) : -1
                })
            })
            enabled: root.phase === FlashStates.Idle || root.phase === FlashStates.Failed
            onActivated: root.selectedPort = currentValue
        }
        ActionButton {
            objectName: "fcFlashProbe"
            text: qsTr("보드 확인")
            enabled: root.selectedPort !== "" && port.currentIndex >= 0 && (root.phase === FlashStates.Idle || root.phase === FlashStates.Failed)
            onClicked: FcFlashController.beginProbe(root.selectedPort)
        }
        Item { Layout.fillWidth: true }
    }
    Text {
        width: parent.width
        wrapMode: Text.Wrap
        color: root.phase === FlashStates.WaitReplug ? Theme.warn : Theme.inkMuted
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        text: {
            switch (root.phase) {
            case FlashStates.Idle: return qsTr("앱이 FC를 부트로더로 넣고 보드 정보를 읽어요.")
            case FlashStates.Probing: return qsTr("포트를 확인하고 있어요")
            case FlashStates.Rebooting: return qsTr("FC를 부트로더로 넣고 있어요")
            case FlashStates.WaitReplug: return qsTr("USB를 뽑았다가 다시 꽂아요 · %1초").arg(FcFlashController.secondsLeft)
            case FlashStates.PickPort: return qsTr("부트로더 포트가 둘 이상이에요. 하나를 골라요.")
            default: return ""
            }
        }
    }
    BusySpinner { visible: root.phase === FlashStates.Probing || root.phase === FlashStates.Rebooting }
    Flow {
        width: parent.width
        spacing: Theme.inset
        visible: root.phase === FlashStates.PickPort
        ComboField { id: pick; objectName: "fcFlashPickPort"; model: FcFlashController.pickPorts }
        ActionButton {
            text: qsTr("이 포트로 계속")
            enabled: pick.currentIndex >= 0
            onClicked: FcFlashController.pickPort(pick.currentText)
        }
    }
    Text {
        width: parent.width
        visible: root.phase === FlashStates.Failed
        text: root.failure
        color: Theme.danger
        wrapMode: Text.Wrap
        font.pixelSize: Theme.fontSize
    }
    ActionButton {
        visible: root.phase === FlashStates.Failed && FcFlashController.canRetry
        text: qsTr("다시 하기")
        onClicked: FcFlashController.retryProbe()
    }
}
