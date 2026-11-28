// [0.6.0 5.11.9] specs/features/0.6.0-fly.md
/** 상단바의 모드 글과 모드 창이에요.
 * 입력: FlyController의 modeText·modes·currentMode·canSetMode예요.
 * 출력: "모드: LOITER"를 보이고, 누르면 그 기체 종류의 모드 목록 창을 열어요. 지금 모드는 체크 표시와 함께 흐리고,
 *       공중에서는 스틱을 따르는 모드도 흐려요. 하나를 고르면 확인 단추가 보이고 길게 누르면 setMode를 불러요.
 * 실패: 바꿀 수 없게 되거나 활성 기체가 바뀌면 창이 닫혀요.
 * 흐름:
 * 1. 모드 단추 — 창을 열고 고를 모드를 비워요.
 * 2. flyModeChoice_* — 고를 모드를 정해요.
 * 3. flyModeConfirm — 다 누르면 FlyController.setMode가 FlyService로 DO_SET_MODE를 보내요.
 */
import QtQuick
import QtQuick.Controls
import Kgc.Ui
import Kgc.Fly

ActionButton {
    objectName: "topbar_fly.mode"
    kind: "secondary"
    text: { const language = translator.language; return FlyController.modeText }
    trailingIcon: "chevron-down"
    enabled: FlyController.canSetMode
    onClicked: popup.visible ? popup.close() : popup.open()
    Popup {
        id: popup
        objectName: "flyModePopup"
        width: Theme.flyModePopupWidth
        y: parent.height + Theme.inset
        padding: Theme.inset
        // 초점을 가져야 Esc로 닫혀요(CloseOnEscape는 초점이 있는 창만 닫아요).
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        background: Rectangle { color: Theme.surface; border.color: Theme.lineStrong; radius: Theme.radiusSmall }
        property double chosen: -1
        onOpened: chosen = -1
        readonly property var chosenEntry: FlyController.modes.find(x => x.customMode === popup.chosen) ?? null
        readonly property string chosenName: chosenEntry ? chosenEntry.name : ""
        readonly property bool chosenChoosable: chosenEntry ? chosenEntry.choosable : false
        Column {
            spacing: Theme.inset
            width: popup.availableWidth
            Flow {
                width: parent.width
                spacing: Theme.choiceGap
                Repeater {
                    model: FlyController.modes
                    ActionButton {
                        required property var modelData
                        objectName: "flyModeChoice_" + modelData.name
                        text: modelData.name
                        kind: modelData.customMode === popup.chosen ? "primary" : "secondary"
                        trailingIcon: modelData.customMode === FlyController.currentMode ? "check" : ""
                        enabled: modelData.customMode !== FlyController.currentMode && modelData.choosable
                        onClicked: popup.chosen = modelData.customMode
                    }
                }
            }
            Text {
                objectName: "flyModeChosen"
                visible: popup.chosen >= 0
                text: qsTr("바꿀 모드: %1").arg(popup.chosenName)
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
                color: Theme.ink
            }
            Row {
                visible: popup.chosen >= 0
                spacing: Theme.inset
                FlyHoldButton {
                    objectName: "flyModeConfirm"
                    text: qsTr("길게 눌러 바꾸기")
                    enabled: FlyController.canSetMode && popup.chosenChoosable
                    onConfirmed: { FlyController.setMode(pressedSysid, popup.chosen); popup.close() }
                }
                ActionButton {
                    objectName: "flyModeCancel"
                    kind: "secondary"
                    text: qsTr("취소")
                    onClicked: popup.chosen = -1
                }
            }
        }
        Connections {
            target: FlyController
            function onReadinessChanged() { if (!FlyController.canSetMode) popup.close() }
            function onVehicleChanged() { popup.close() }
        }
    }
}
