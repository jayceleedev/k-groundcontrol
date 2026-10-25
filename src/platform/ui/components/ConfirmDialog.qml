// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 확인과 취소를 분명히 나눈 대화 상자예요.
 * 입력: title, text, danger, confirmText, open(), close()와 accepted(), rejected()를 사용해요.
 * 출력: 확인하면 accepted, 취소하면 rejected를 내고 창을 닫아요.
 * 실패: 확인한 작업은 실행하지 않으므로 실제 작업 실패는 호출자가 처리해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.Dialog {
    id: dialog
    property string text
    property bool danger: false
    property string confirmText: qsTr("확인")
    modal: true
    padding: Theme.panelInset
    implicitWidth: Theme.confirmDialogWidth
    background: Rectangle { color: Theme.surface; radius: Theme.radiusLarge; border.color: Theme.line }
    header: Text { text: dialog.title; color: Theme.ink; font.pixelSize: Theme.titleSize; font.bold: true; padding: Theme.panelInset; wrapMode: Text.Wrap }
    contentItem: Text { text: dialog.text; color: Theme.inkMuted; font.pixelSize: Theme.fontSize; wrapMode: Text.Wrap }
    footer: Row {
        spacing: Theme.inset
        padding: Theme.panelInset
        ActionButton { text: dialog.confirmText; kind: dialog.danger ? "danger" : "primary"; onClicked: dialog.accept() }
        ActionButton { text: qsTr("취소"); kind: "secondary"; onClicked: dialog.reject() }
    }
}
