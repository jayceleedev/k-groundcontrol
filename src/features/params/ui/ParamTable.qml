// [0.3.0 6.8 6.8-1] specs/features/0.3.0-params.md
/** 파라미터를 가상화한 표로 보여 주고 값 확정을 맡아요.
 * 입력: ParamsController.model이에요.
 * 출력: 행 선택과 대기값 요청, 값 오류 글 editError예요.
 * 실패: 나쁜 입력은 editError로, 범위 밖 값은 확인 창으로 물어요.
 * 흐름:
 * 1. commit → ParamsController.checkValue — 입력을 검사해요.
 * 2. ParamsController.setPending — 값을 대기값으로 정해요. 범위 밖이면 먼저 물어요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import Kgc.Params

Item {
    id: root
    property string editError: ""
    readonly property bool dialogOpen: rangeDialog.visible || bitsDialog.visible
    signal resetEditor()
    signal focusEditor()
    /** 다른 행을 골라요. 입력: 이름 name이에요. 출력: 고른 행이 바뀌어요. 실패: 지금 값에 오류가 있으면 그대로 있어요. */
    function selectRow(name: string) {
        if (ParamsController.selectedName === name) return
        commitEditor()
        if (editError === "") ParamsController.selectedName = name
        else focusEditor()
    }
    /** 편집 중인 값을 초점 이동으로 확정해요. 입력: 없어요. 출력: 값이 확정돼요. 실패: editError를 채워요. */
    function commitEditor() {
        forceActiveFocus()
        // 초점이 옮겨 가는 도중에 창이 열리면 이 함수가 창의 초점을 뺏으므로 돌려줘서 Esc가 창으로 가게 해요.
        if (rangeDialog.visible) rangeDialog.forceActiveFocus()
    }
    /** 입력한 글자를 확정해요. 입력: 이름 name과 글자 text예요. 출력: 대기값이 바뀌어요. 실패: editError를 채워요. */
    function commit(name: string, text: string) {
        const result = ParamsController.checkValue(name, text)
        editError = result.error ?? ""
        if (result.unchanged || editError !== "") {
            if (result.unchanged) resetEditor()
            return
        }
        if (!result.ask) {
            ParamsController.setPending(name, result.value)
            resetEditor()
            return
        }
        rangeDialog.paramName = name
        rangeDialog.value = result.value
        rangeDialog.range = result.range
        rangeDialog.open()
    }
    /** 입력을 버려요. 입력: 없어요. 출력: 오류 글이 지워져요. 실패: 없어요. */
    function cancelEdit() {
        editError = ""
        resetEditor()
    }
    /** 비트 창을 열어요. 입력: 이름 name이에요. 출력: 창이 열려요. 실패: 없어요. */
    function openBits(name: string) { bitsDialog.show(name) }
    component HeaderText: Text {
        height: parent.height
        leftPadding: Theme.inset
        verticalAlignment: Text.AlignVCenter
        color: Theme.inkMuted
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        elide: Text.ElideRight
    }
    Row {
        id: headerRow
        anchors.left: parent.left
        anchors.right: parent.right
        height: Theme.paramRowHeight
        HeaderText { width: Theme.paramNameWidth; text: qsTr("이름") }
        HeaderText { width: Theme.paramValueWidth; text: qsTr("값") }
        HeaderText { width: Theme.paramDefaultWidth; visible: ParamsController.hasDefaults; text: qsTr("기본값") }
        HeaderText { width: Theme.paramUnitWidth; text: qsTr("단위") }
        HeaderText { width: Theme.paramRangeWidth; text: qsTr("범위·선택지") }
        HeaderText { text: qsTr("설명") }
    }
    ListView {
        id: list
        objectName: "paramsTable"
        anchors.top: headerRow.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        reuseItems: true
        model: ParamsController.model
        // 편집기가 화면 밖으로 나가면 사라지므로 움직이기 전에 확정해요.
        onMovementStarted: root.commitEditor()
        Basic.ScrollBar.vertical: Basic.ScrollBar { implicitWidth: Theme.scrollBarWidth }
        delegate: ParamRow {
            width: list.width - Theme.scrollGutter
            table: root
        }
    }
    // 고른 행이 사라지면 고칠 편집기도 없으니 오류 글이 남아 막히지 않게 지워요.
    Connections {
        target: ParamsController
        function onSelectedChanged() { if (ParamsController.selectedName === "") root.editError = "" }
    }
    ConfirmDialog {
        id: rangeDialog
        property string paramName
        property double value
        property string range
        parent: Basic.Overlay.overlay
        anchors.centerIn: parent
        title: qsTr("범위를 벗어났어요")
        text: qsTr("%1의 범위는 %2예요. 이 값으로 둘까요?").arg(paramName).arg(range)
        onAccepted: { ParamsController.setPending(paramName, value); root.resetEditor() }
        onRejected: root.resetEditor()
        onClosed: root.focusEditor()
    }
    BitmaskDialog {
        id: bitsDialog
        parent: Basic.Overlay.overlay
        anchors.centerIn: parent
        onValueChosen: (name, value) => root.commit(name, String(value))
    }
}
