// [0.2.0 T19–T23 T29 T30] specs/features/0.2.0-fcflash.md
/** 굽기의 단계와 돌아온 기체의 확인 결과를 보여요.
 * 입력: Job, 굽기 단계, 재연결 결과예요.
 * 출력: 진행률과 재시도·처음으로 요청이에요.
 * 실패: 포트 유실과 단계별 실패를 현재 언어로 보여요.
 * 흐름:
 * 1. JobView — 취소 요청을 서비스에 넘겨요.
 * 2. FcFlashController.retryFlash·reset — 다시 굽거나 부팅 없이 처음으로 돌아가요.
 */
import QtQuick
import Kgc.Ui
import Kgc.FcFlash

Column {
    id: root
    spacing: Theme.panelInset
    readonly property int phase: FcFlashController.phase
    readonly property bool finished: phase === FlashStates.Done || phase === FlashStates.Mismatch || phase === FlashStates.NoReturn
    // 끝난 뒤의 결과 카드 내용이에요. 결과가 없는 단계에서는 null이에요.
    readonly property var result: {
        const language = translator.language
        const name = FcFlashController.vehicleFirmwareName
        const version = FcFlashController.vehicleVersion
        const reached = version ? name + " " + version : name
        switch (phase) {
        case FlashStates.Done:
            return {icon: "circle-check", color: Theme.ok, title: qsTr("올리기 완료"), detail: reached}
        case FlashStates.Mismatch:
            return {icon: "alert-triangle", color: Theme.warn, title: qsTr("선택한 펌웨어와 불일치"), detail: reached}
        case FlashStates.NoReturn:
            return {icon: "plug-connected", color: Theme.warn, title: qsTr("펌웨어 굽기 완료"),
                detail: qsTr("FC 재시작 안 됨. USB 케이블 분리 후 재연결 또는 리셋 버튼 누른 뒤 연결")}
        case FlashStates.Failed:
            return {icon: "circle-x", color: Theme.danger, title: FcFlashController.failureText(), detail: ""}
        default:
            return null
        }
    }
    Row {
        width: parent.width
        spacing: Theme.panelInset
        JobView {
            width: Theme.inputWidth
            anchors.verticalCenter: parent.verticalCenter
            visible: root.phase === FlashStates.Flashing && FcFlashController.flashStage < 4
            job: FcFlashController.job
            showError: false
            ring: true
        }
        StepList {
            anchors.verticalCenter: parent.verticalCenter
            model: {
                const labels = [qsTr("받기"), qsTr("지우기"), qsTr("쓰기"), qsTr("검증"), qsTr("재부팅"), qsTr("재연결")]
                const first = FcFlashController.source === FlashStates.LocalFile ? 1 : 0
                const current = FcFlashController.flashStage
                let rows = []
                for (let i = first; i < labels.length; ++i) {
                    let state = i < current ? "done" : i === current ? "current" : "pending"
                    if (i === current && root.phase === FlashStates.Failed) state = "failed"
                    if (i === 5 && root.phase === FlashStates.NoReturn) state = "pending"
                    rows.push({text: labels[i], state: state})
                }
                return rows
            }
        }
    }
    BusySpinner { visible: root.phase === FlashStates.Reconnecting }
    Text {
        width: parent.width
        visible: root.phase === FlashStates.Reconnecting
        wrapMode: Text.Wrap
        text: qsTr("FC 재연결 대기 중")
        color: Theme.warn
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
    }
    ResultCard {
        width: parent.width
        visible: root.result !== null
        icon: root.result?.icon ?? ""
        iconColor: root.result?.color ?? Theme.ink
        title: root.result?.title ?? ""
        detail: root.result?.detail ?? ""
    }
    Flow {
        width: parent.width
        spacing: Theme.inset
        ActionButton {
            visible: root.phase === FlashStates.Failed && FcFlashController.canRetry
            text: qsTr("다시 굽기")
            onClicked: FcFlashController.retryFlash()
        }
        ActionButton {
            visible: root.finished || root.phase === FlashStates.Failed
            text: qsTr("처음으로")
            kind: "secondary"
            onClicked: FcFlashController.reset()
        }
    }
}
