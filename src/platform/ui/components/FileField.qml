// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 경로 입력과 파일 선택 요청을 함께 제공해요.
 * 입력: text, placeholderText와 browseRequested()를 사용해요. 실제 파일 선택은 호출자가 맡아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Row {
    id: control
    property alias text: field.text
    property alias placeholderText: field.placeholderText
    signal browseRequested()
    spacing: Theme.inset
    TextBox { id: field }
    ActionButton { kind: "secondary"; text: qsTr("찾아보기"); onClicked: control.browseRequested() }
}
