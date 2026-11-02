// [0.3.0 6.8] specs/features/0.3.0-params.md
/** 비트마다 체크해 값을 만들어요.
 * 입력: show(name)으로 고를 파라미터를 받아요.
 * 출력: 확인하면 valueChosen(name, value)를 내요. 값은 지금 값에서 목록의 비트만 바꾼 것이고, 계산은 컨트롤러가 값의 형 폭으로 해요.
 * 실패: 없어요.
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import Kgc.Params

Basic.Dialog {
    id: dialog
    objectName: "paramsBitDialog"
    property string paramName
    property var bits: []
    // 행이 화면 밖으로 나갔다 돌아와도 체크가 남도록 창이 비트별 상태를 들어요.
    property var checkedBits: ({})
    signal valueChosen(string name, double value)
    /** 창을 열어요. 입력: 파라미터 이름 name이에요. 출력: 지금 값의 비트가 체크돼요. 실패: 없어요. */
    function show(name: string) {
        paramName = name
        const model = ParamsController.bitModel(name)
        const chosen = {}
        for (const bit of model) chosen[bit.bit] = bit.checked
        // 목록이 행을 만들 때 읽으므로 체크 상태를 먼저 넣어요.
        checkedBits = chosen
        bits = model
        open()
    }
    title: qsTr("%1 비트 고르기").arg(paramName)
    modal: true
    padding: Theme.panelInset
    implicitWidth: Theme.confirmDialogWidth
    background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.color: Theme.line }
    header: Text { text: dialog.title; color: Theme.ink; font.pixelSize: Theme.titleSize; font.bold: true; padding: Theme.panelInset; wrapMode: Text.Wrap }
    contentItem: ListView {
        id: list
        implicitHeight: Math.min(contentHeight, Theme.dialogHeight / 2)
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        model: dialog.bits
        Basic.ScrollBar.vertical: Basic.ScrollBar { implicitWidth: Theme.scrollBarWidth }
        delegate: CheckRow {
            required property var modelData
            width: list.width - Theme.scrollGutter
            tristate: false
            text: `${modelData.bit} ${modelData.label}`
            checked: dialog.checkedBits[modelData.bit] === true
            onToggled: dialog.checkedBits[modelData.bit] = checked
        }
    }
    footer: Row {
        spacing: Theme.inset
        padding: Theme.panelInset
        ActionButton {
            objectName: "paramsBitConfirm"
            text: qsTr("확인")
            onClicked: {
                const chosen = dialog.bits.map(bit => bit.bit).filter(bit => dialog.checkedBits[bit] === true)
                dialog.valueChosen(dialog.paramName, ParamsController.bitsValue(dialog.paramName, chosen))
                dialog.accept()
            }
        }
        ActionButton { text: qsTr("취소"); kind: "secondary"; onClicked: dialog.reject() }
    }
}
