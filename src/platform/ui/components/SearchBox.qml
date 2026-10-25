// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 검색어를 쉽게 지울 수 있는 입력란이에요.
 * 입력: text, placeholderText와 editingFinished를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

TextBox {
    id: control
    leftPadding: Theme.menuHeight
    rightPadding: Theme.menuHeight
    IconGlyph { name: "search"; x: Theme.inset; anchors.verticalCenter: parent.verticalCenter }
    IconButton { name: "x"; hint: qsTr("검색어 지우기"); anchors.right: parent.right; visible: control.text.length > 0; onClicked: { control.clear(); control.forceActiveFocus() } }
}
