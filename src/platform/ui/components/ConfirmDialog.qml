// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 확인과 취소를 분명히 나눈 대화 상자예요.
 * 입력: title, text, danger, confirmText, open(), close()와 accepted(), rejected()를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
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
    implicitWidth: Theme.languageWidth * 2
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
