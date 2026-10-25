// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 경로 입력과 파일 선택 요청을 함께 제공해요.
 * 입력: text, placeholderText와 browseRequested()·editingFinished()를 사용해요. 실제 파일 선택은 호출자가 맡아요.
 * 출력: 입력한 경로를 text로 내고 찾아보기 단추가 browseRequested를 내요.
 * 실패: 파일 존재 여부를 검사하지 않아요. 파일 선택 실패는 호출자가 처리해요.
 */
import QtQuick
import Kgc.Ui

Row {
    id: control
    property alias text: field.text
    property alias placeholderText: field.placeholderText
    signal browseRequested()
    signal editingFinished()
    spacing: Theme.inset
    TextBox { id: field; onEditingFinished: control.editingFinished() }
    ActionButton { kind: "secondary"; text: qsTr("찾아보기"); onClicked: control.browseRequested() }
}
