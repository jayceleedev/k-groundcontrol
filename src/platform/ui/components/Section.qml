// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 내용의 시작을 머리글과 선으로 표시해요.
 * 입력: text를 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Column {
    id: section
    property string text
    spacing: Theme.inset
    Text { text: section.text; color: Theme.ink; font.pixelSize: Theme.titleSize; font.bold: true }
    Rectangle { width: parent.width; height: Theme.lineWidth; color: Theme.lineStrong }
}
