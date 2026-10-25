// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 아이콘과 조작의 뜻을 작은 풍선으로 보여줘요.
 * 입력: text와 visible을 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.ToolTip {
    id: control
    padding: Theme.inset
    font.pixelSize: Theme.fontSize
    contentItem: Text { text: control.text; font: control.font; color: Theme.inkInverse }
    background: Rectangle { color: Theme.ink; radius: Theme.radiusSmall }
}
