// [0.6.0 §12 2] specs/features/0.6.0-fly.md
// [0.6.0 5.11.9] specs/features/0.6.0-fly.md
/** 상단바의 활성 기체 글과 기다림 표시, 비행 명령 실패 토스트예요.
 * 입력: FlyController의 vehicleText·busy·toast예요.
 * 출력: "기체 1"·"기체 없음"과, 명령이 응답을 기다리는 동안 도는 표시(자리는 늘 차지)를 보여요.
 *       실패 글은 앱 창 가로 가운데, 상단바 바로 아래에 6초 동안 보여요. 열린 토스트에 늦게 온 기체 이유 줄이 붙으면
 *       글만 바꾸고 처음 뜬 때부터 6초에 닫혀요.
 * 실패: 없어요.
 */
import QtQuick
import QtQuick.Controls
import Kgc.Ui
import Kgc.Fly

Row {
    objectName: "topbar_fly.vehicle"
    spacing: Theme.inset
    height: Theme.controlHeight
    Text {
        objectName: "flyVehicleText"
        text: { const language = translator.language; return FlyController.vehicleText }
        color: Theme.ink
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        height: Theme.controlHeight
        verticalAlignment: Text.AlignVCenter
    }
    BusySpinner {
        objectName: "flyBusy"
        running: FlyController.busy
        opacity: FlyController.busy ? 1 : 0
        anchors.verticalCenter: parent.verticalCenter
    }
    Popup {
        id: toastPopup
        objectName: "flyToast"
        parent: Overlay.overlay
        x: (parent.width - width) / 2
        y: Theme.topBarHeight + Theme.inset
        width: Math.min(implicitWidth, Theme.dialogWidth)
        modal: false
        focus: false
        closePolicy: Popup.NoAutoClose
        padding: 0
        background: null
        contentItem: Toast { id: toast; objectName: "flyToastText"; wrapMode: Text.Wrap }
    }
    TimerBox { id: toastTimer; interval: Theme.flyToastMs; onTriggered: toastPopup.close() }
    Connections {
        target: FlyController
        function onToast(text) {
            // 열린 토스트에 늦게 온 기체 이유 줄만 덧붙는 글이면 글만 바꾸고 6초 닫힘 시간은 다시 세지 않아요(§12 고침 2).
            if (toastPopup.opened && text.startsWith(toast.text + "\n")) { toast.text = text; return }
            toast.text = text
            toastPopup.open()
            toastTimer.restart()
        }
    }
}
