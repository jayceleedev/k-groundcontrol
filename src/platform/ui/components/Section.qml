// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 내용의 시작을 머리글과 선으로 표시해요.
 * 입력: text와 SettingsPage에서 쓸 category 식별자를 받아요.
 * 출력: text 머리글 아래에 구분선을 그려 내용의 시작을 표시해요.
 * 실패: 빈 text에도 구분선은 남고 오류 신호는 내지 않아요.
 */
import QtQuick
import Kgc.Ui

Column {
    id: section
    property string category: ""
    property string text
    spacing: Theme.inset
    Text { text: section.text; color: Theme.ink; font.pixelSize: Theme.titleSize; font.bold: true }
    Rectangle { width: parent.width; height: Theme.lineWidth; color: Theme.lineStrong }
}
