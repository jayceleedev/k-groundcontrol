// [0.6.0 5.11.9] specs/features/0.6.0-fly.md
/** 상단바의 이륙 단추와 이륙 창이에요.
 * 입력: FlyController의 canTakeoff·takeoffKind·takeoffAltitude·takeoffFixedText와 고도 한계예요.
 * 출력: 높이를 넣는 이륙이면 "이륙 높이(지금 자리 위)" 칸을, 기체 설정 고도 이륙이면 그 안내 글을 보이고,
 *       확인 단추를 길게 누르면 takeoff를 불러요. 높이 칸은 창을 열 때 마지막으로 보낸 높이로 채워요.
 * 실패: 높이가 한계 밖이면 확인 단추가 흐려요. 이륙할 수 없게 되거나 활성 기체가 바뀌면 창이 닫혀요.
 * 흐름:
 * 1. 이륙 단추 — 창을 열고 높이 칸을 채워요.
 * 2. flyTakeoffConfirm — 누르는 순간 높이 칸이 확정되고, 다 누르면 FlyController.takeoff가 FlyService로 이륙 단계를 보내요.
 */
import QtQuick
import QtQuick.Controls
import Kgc.Ui
import Kgc.Fly

ActionButton {
    objectName: "topbar_fly.takeoff"
    kind: "secondary"
    text: qsTr("이륙")
    icon.source: "qrc:/kgc/icons/plane-departure.svg"
    enabled: FlyController.canTakeoff
    onClicked: popup.visible ? popup.close() : popup.open()
    Popup {
        id: popup
        objectName: "flyTakeoffWindow"
        width: Theme.flyWindowWidth
        y: parent.height + Theme.inset
        padding: Theme.inset
        // 초점을 가져야 Esc로 닫혀요(CloseOnEscape는 초점이 있는 창만 닫아요).
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        background: Rectangle { color: Theme.surface; border.color: Theme.lineStrong; radius: Theme.radiusSmall }
        // 높이는 열 때 한 번 넣어요(바인딩이 아니라 적은 값을 지우지 않게).
        onOpened: altitudeBox.value = FlyController.takeoffAltitude
        Column {
            width: popup.availableWidth
            spacing: Theme.inset
            Label { visible: FlyController.takeoffKind === 1; text: qsTr("이륙 높이(지금 자리 위)"); color: Theme.inkMuted }
            NumberBox {
                id: altitudeBox
                objectName: "flyTakeoffAltitude"
                visible: FlyController.takeoffKind === 1
                from: FlyController.guidedAltitudeFrom
                to: FlyController.guidedAltitudeTo
                decimals: 1
                unit: qsTr("m")
                width: parent.width
            }
            Text {
                objectName: "flyTakeoffFixed"
                visible: FlyController.takeoffKind === 2
                text: { const language = translator.language; return FlyController.takeoffFixedText }
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
                color: Theme.ink
                wrapMode: Text.Wrap
                width: parent.width
            }
            Row {
                spacing: Theme.inset
                FlyHoldButton {
                    objectName: "flyTakeoffConfirm"
                    text: qsTr("길게 눌러 이륙")
                    enabled: FlyController.canTakeoff && (FlyController.takeoffKind !== 1 || altitudeBox.acceptableInput)
                    onConfirmed: { FlyController.takeoff(pressedSysid, FlyController.takeoffKind === 1 ? altitudeBox.value : NaN); popup.close() }
                }
                ActionButton {
                    objectName: "flyTakeoffCancel"
                    kind: "secondary"
                    text: qsTr("취소")
                    onClicked: popup.close()
                }
            }
        }
        Connections {
            target: FlyController
            function onReadinessChanged() { if (!FlyController.canTakeoff) popup.close() }
            function onVehicleChanged() { popup.close() }
        }
    }
}
