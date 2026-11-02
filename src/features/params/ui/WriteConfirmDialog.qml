// [0.3.0 6.8] specs/features/0.3.0-params.md
/** 쓸 값을 보여 주고 확인을 받아요.
 * 입력: 표 줄 rows와 길게 눌러 확인할지 정하는 hold예요.
 * 출력: 확인하면 accepted를 내요.
 * 실패: 쓰기 자체의 실패는 컨트롤러가 알려요.
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.Dialog {
    id: dialog
    objectName: "paramsWriteDialog"
    property var rows: []
    property bool hold: false
    title: qsTr("값 %1개를 쓸까요?").arg(rows.length)
    modal: true
    padding: Theme.panelInset
    implicitWidth: Theme.confirmDialogWidth
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
        RowLayout {
            Cell { text: qsTr("이름"); color: Theme.inkMuted }
            Cell { text: qsTr("지금"); color: Theme.inkMuted }
            Cell { text: qsTr("새 값"); color: Theme.inkMuted }
        }
        ListView {
            id: list
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(contentHeight, Theme.comboPopupHeight)
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            model: dialog.rows
            delegate: RowLayout {
                id: line
                required property var modelData
                width: list.width
                Cell { text: line.modelData.name; font.family: Theme.monoFamily }
                Cell { text: line.modelData.current }
                Cell { text: line.modelData.next; color: Theme.ok }
            }
        }
        Text {
            Layout.fillWidth: true
            text: qsTr("쓰기 전에 전체 값을 백업해요.")
            color: Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            wrapMode: Text.Wrap
        }
    }
    footer: Row {
        spacing: Theme.inset
        padding: Theme.panelInset
        ActionButton {
            objectName: "paramsWriteConfirm"
            visible: !dialog.hold
            text: qsTr("쓰기")
            onClicked: dialog.accept()
        }
        HoldToConfirm {
            objectName: "paramsWriteHold"
            visible: dialog.hold
            text: qsTr("쓰기")
            onConfirmed: dialog.accept()
        }
        ActionButton { text: qsTr("취소"); kind: "secondary"; onClicked: dialog.reject() }
    }
}
