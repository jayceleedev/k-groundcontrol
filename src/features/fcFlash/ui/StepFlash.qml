// [0.2.0 T19–T23] specs/features/0.2.0-fcflash.md
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
    StepList {
        model: {
            const labels = [qsTr("내려받기"), qsTr("지우기"), qsTr("쓰기"), qsTr("검증"), qsTr("재부팅"), qsTr("재연결")]
            const first = FcFlashController.source === FlashStates.LocalFile ? 1 : 0
            const current = FcFlashController.flashStage
            let rows = []
            for (let i = first; i < labels.length; ++i) {
                let state = i < current ? "done" : i === current ? "current" : "pending"
                if (i === current && root.phase === FlashStates.Failed) state = "failed"
                if (i === 5 && root.phase === FlashStates.NoReturn) state = "failed"
                rows.push({text: labels[i], state: state})
            }
            return rows
        }
    }
    JobView {
        width: parent.width
        visible: root.phase === FlashStates.Flashing && FcFlashController.flashStage < 4
        job: FcFlashController.job
        showError: false
    }
    BusySpinner { visible: root.phase === FlashStates.Reconnecting }
    Text {
        width: parent.width
        wrapMode: Text.Wrap
        color: root.phase === FlashStates.Done ? Theme.ok : root.phase === FlashStates.Failed ? Theme.danger : Theme.warn
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        text: {
            const language = translator.language
            const name = FcFlashController.vehicleFirmwareName
            const version = FcFlashController.vehicleVersion
            switch (root.phase) {
            case FlashStates.Reconnecting: return qsTr("FC가 돌아오길 기다리고 있어요")
            case FlashStates.Done: return version ? qsTr("올라갔어요: %1 %2").arg(name).arg(version) : qsTr("올라갔어요: %1").arg(name)
            case FlashStates.Mismatch: return qsTr("올린 펌웨어와 달라요: %1 %2").arg(name).arg(version)
            case FlashStates.NoReturn: return qsTr("굽기는 끝났지만 FC가 돌아오지 않았어요. 포트를 골라 직접 연결해요.")
            case FlashStates.Failed: return FcFlashController.failureText()
            default: return ""
            }
        }
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
