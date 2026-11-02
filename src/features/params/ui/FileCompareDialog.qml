// [0.3.0 6.8] specs/features/0.3.0-params.md
/** 파일과 FC의 다른 값을 보여 주고 올릴 값을 골라요.
 * 입력: ParamsController.diffRows와 diffSummary예요.
 * 출력: 고른 값을 ParamsController.applyFile로 넘겨요.
 * 실패: 바뀌는 값이 없으면 올리기 단추가 비활성이에요.
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import Kgc.Params

Basic.Dialog {
    id: dialog
    objectName: "paramsCompareDialog"
    property var unchecked: ({})
    readonly property var rows: ParamsController.diffRows
    /** 모두 체크한 상태로 창을 열어요. 입력: 없어요. 출력: 창이 열려요. 실패: 없어요. */
    function show() {
        unchecked = {}
        open()
    }
    title: qsTr("파일과 FC 비교")
    modal: true
    padding: Theme.panelInset
    implicitWidth: Theme.dialogWidth
    background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.color: Theme.line }
    header: Text { text: dialog.title; color: Theme.ink; font.pixelSize: Theme.titleSize; font.bold: true; padding: Theme.panelInset; wrapMode: Text.Wrap }
    component Cell: Text {
        Layout.fillWidth: true
        Layout.preferredWidth: 1
        color: Theme.ink
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        elide: Text.ElideRight
    }
    contentItem: ColumnLayout {
        spacing: Theme.inset
        Text {
            objectName: "paramsCompareSummary"
            Layout.fillWidth: true
            text: ParamsController.diffSummary
            color: Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            wrapMode: Text.Wrap
        }
        RowLayout {
            Item { implicitWidth: Theme.controlHeight }
            Cell { text: qsTr("이름"); color: Theme.inkMuted }
            Cell { text: qsTr("FC 값"); color: Theme.inkMuted }
            Cell { text: qsTr("파일 값"); color: Theme.inkMuted }
        }
        ListView {
            id: list
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(contentHeight, Theme.dialogHeight / 2)
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            model: dialog.rows
            Basic.ScrollBar.vertical: Basic.ScrollBar { implicitWidth: Theme.scrollBarWidth }
            delegate: RowLayout {
                id: line
                required property var modelData
                width: list.width - Theme.scrollGutter
                CheckRow {
                    implicitWidth: Theme.controlHeight
                    tristate: false
                    checked: dialog.unchecked[line.modelData.name] !== true
                    onToggled: {
                        const next = Object.assign({}, dialog.unchecked)
                        if (checked) delete next[line.modelData.name]
                        else next[line.modelData.name] = true
                        dialog.unchecked = next
                    }
                }
                Cell { text: line.modelData.name; font.family: Theme.monoFamily }
                Cell { text: line.modelData.current }
                Cell { text: line.modelData.file }
            }
        }
    }
    footer: Row {
        spacing: Theme.inset
        padding: Theme.panelInset
        ActionButton {
            objectName: "paramsCompareApply"
            text: qsTr("고른 값 올리기")
            enabled: dialog.rows.length > 0
            onClicked: {
                ParamsController.applyFile(dialog.rows.map(row => row.name).filter(name => dialog.unchecked[name] !== true))
                dialog.accept()
            }
        }
        ActionButton { text: qsTr("취소"); kind: "secondary"; onClicked: dialog.reject() }
    }
}
