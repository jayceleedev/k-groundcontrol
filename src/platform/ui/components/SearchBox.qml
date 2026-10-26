// [0.0.3 T18] specs/features/0.0.3-windows.md
// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 검색어를 쉽게 지울 수 있는 입력란이에요.
 * 입력: text, placeholderText와 editingFinished를 사용해요.
 * 출력: 편집한 text를 제공하고 지우기 단추는 검색어를 비운 뒤 입력란에 초점을 돌려요.
 *       enabled가 거짓이면 ActionButton처럼 돋보기 아이콘을 inkDisabled로 흐리게 그려요.
 * 실패: 빈 검색어에는 지우기 단추가 나타나지 않아요.
 */
import QtQuick
import Kgc.Ui

TextBox {
    id: control
    leftPadding: Theme.controlHeight
    rightPadding: Theme.controlHeight
    IconGlyph { name: "search"; color: control.enabled ? Theme.ink : Theme.inkDisabled; x: Theme.inset; anchors.verticalCenter: parent.verticalCenter }
    IconButton { flat: true; name: "x"; hint: qsTr("검색어 지우기"); anchors.right: parent.right; anchors.rightMargin: Theme.inset; anchors.verticalCenter: parent.verticalCenter; visible: control.text.length > 0; onClicked: { control.clear(); control.forceActiveFocus() } }
}
