// [0.3.0 6.8 6.8-1] specs/features/0.3.0-params.md
/** 표의 한 행이에요. 고른 행의 값 칸은 종류에 맞는 편집기가 돼요.
 * 입력: Params 역할 값과 값 확정을 맡는 table이에요.
 * 출력: 행 선택과 table.commit 요청이에요.
 * 실패: 읽기 전용 행은 글자만 보여요.
 */
import QtQuick
import Kgc.Ui
import Kgc.Params

Rectangle {
    id: row
    required property string name
    required property string valueText
    required property string pendingText
    required property bool hasPending
    required property string optionText
    required property string defaultText
    required property string units
    required property string rangeText
    required property string title
    required property string description
    required property bool readOnly
    required property bool rebootRequired
    required property string kind
    required property ParamTable table
    readonly property bool chosen: ParamsController.selectedName === name
    readonly property string shownText: hasPending ? pendingText : valueText
    // 재사용 대기 중인 행은 편집기를 내려서 초점과 입력이 남지 않게 해요.
    property bool pooled: false
    readonly property var editorItem: editor.item
    ListView.onPooled: pooled = true
    ListView.onReused: pooled = false
    /** 고른 행의 편집기에 초점을 줘요. 입력: 없어요. 출력: 초점이 옮겨 가요. 실패: 편집기가 없으면 아무 일도 없어요. */
    function focusEditor() { if (editorItem) editorItem.takeFocus() }
    Component.onDestruction: if (chosen && table) table.editError = ""
    objectName: "paramRow_" + name
    height: Theme.paramRowHeight
    color: chosen ? Theme.surfaceAlt : "transparent"
    TapHandler {
        onTapped: {
            row.table.selectRow(row.name)
            // 확정이 범위 확인 창을 열었으면 창이 초점을 쥐어야 Esc가 창으로 가요.
            if (!row.table.dialogOpen) row.focusEditor()
        }
    }
    Connections {
        target: row.table
        function onFocusEditor() { if (row.chosen) row.focusEditor() }
    }
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: Theme.lineWidth
        color: Theme.line
    }
    component CellText: Text {
        height: parent.height
        leftPadding: Theme.inset
        verticalAlignment: Text.AlignVCenter
        color: Theme.inkMuted
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        elide: Text.ElideRight
    }
    Row {
        anchors.fill: parent
        CellText {
            width: Theme.paramNameWidth
            text: row.name
            color: Theme.ink
            font.family: Theme.monoFamily
        }
        Item {
            width: Theme.paramValueWidth
            height: parent.height
            Rectangle {
                anchors.fill: parent
                visible: row.hasPending
                color: Theme.okSoft
            }
            CellText {
                objectName: "paramValue_" + row.name
                anchors.left: parent.left
                anchors.right: rebootMark.active ? rebootMark.left : parent.right
                visible: !editor.active
                text: row.optionText !== "" ? row.optionText : row.shownText
                color: row.hasPending ? Theme.ok : Theme.ink
            }
            // 아이콘 효과가 무거워서 재부팅 표시가 필요한 행에만 만들어요.
            Loader {
                id: rebootMark
                anchors.right: parent.right
                anchors.rightMargin: Theme.choiceGap
                anchors.verticalCenter: parent.verticalCenter
                width: Theme.flatIconSize
                height: Theme.flatIconSize
                active: row.rebootRequired && !editor.active
                sourceComponent: Component {
                    IconGlyph {
                        name: "refresh"
                        color: Theme.warn
                    }
                }
            }
            Loader {
                id: editor
                anchors.fill: parent
                active: row.chosen && !row.readOnly && !row.pooled
                onActiveChanged: if (!active) row.table.editError = ""
                sourceComponent: row.kind === "option" ? optionEditor : numberEditor
            }
        }
        CellText { width: Theme.paramDefaultWidth; visible: ParamsController.hasDefaults; text: row.defaultText }
        CellText { width: Theme.paramUnitWidth; text: row.units }
        CellText { width: Theme.paramRangeWidth; text: row.rangeText }
        CellText { width: row.width - x; text: row.title !== "" ? row.title : row.description }
    }
    // 11자리 음수도 다 보이도록 값 칸은 그대로 두고, 비트 단추는 고른 행에서만 다음 열 위에 얹어요.
    Loader {
        x: Theme.paramNameWidth + Theme.paramValueWidth
        height: row.height
        active: editor.active && row.kind === "bitmask"
        sourceComponent: Component {
            Rectangle {
                color: Theme.surfaceAlt
                implicitWidth: bitButton.implicitWidth + Theme.choiceGap * 2
                ActionButton {
                    id: bitButton
                    objectName: "paramBits"
                    anchors.centerIn: parent
                    height: parent.height
                    kind: "ghost"
                    padding: Theme.choiceGap
                    font.pixelSize: Theme.captionSize
                    text: qsTr("비트 고르기")
                    onClicked: row.table.openBits(row.name)
                }
            }
        }
    }
    component ValueBox: TextBox {
        id: box
        /** 초점을 받고 글자를 고르게 해요. 입력: 없어요. 출력: 초점이 와요. 실패: 없어요. */
        function takeFocus() { forceActiveFocus(); selectAll() }
        text: row.shownText
        color: row.hasPending ? Theme.ok : Theme.ink
        onEditingFinished: row.table.commit(row.name, text)
        Keys.onEscapePressed: row.table.cancelEdit()
        Connections {
            target: row
            function onShownTextChanged() { box.text = row.shownText }
        }
        Connections {
            target: row.table
            function onResetEditor() { box.text = row.shownText }
        }
    }
    Component {
        id: numberEditor
        ValueBox { objectName: "paramEditor" }
    }
    Component {
        id: optionEditor
        ComboField {
            id: combo
            property var choices: ParamsController.optionModel(row.name)
            /** 값이 바뀌면 목록을 다시 읽어요. 입력: 없어요. 출력: 목록과 고른 줄이 지금 값을 따라요. 실패: 없어요. */
            function reload() {
                choices = ParamsController.optionModel(row.name)
                currentIndex = choices.current
            }
            /** 초점을 받아요. 입력: 없어요. 출력: 초점이 와요. 실패: 없어요. */
            function takeFocus() { forceActiveFocus() }
            objectName: "paramEditor"
            textRole: "text"
            valueRole: "value"
            model: choices.items
            currentIndex: choices.current
            onActivated: index => row.table.commit(row.name, String(choices.items[index].value))
            Keys.onEscapePressed: row.table.cancelEdit()
            Connections {
                target: row.table
                function onResetEditor() { combo.reload() }
            }
            Connections {
                target: row
                function onShownTextChanged() { combo.reload() }
            }
        }
    }
}
