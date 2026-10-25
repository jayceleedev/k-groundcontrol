// [0.0.2 T12 T23] specs/features/0.0.2-theme.md
/** 아이콘과 조작의 뜻을 작은 풍선으로 보여줘요.
 * 입력: text와 visible을 받아요.
 * 출력: visible인 동안 text 설명 풍선을 역상 글자 색으로 표시해요.
 * 실패: 설명 내용의 유무와 표시 시점은 이 풍선을 여는 컨트롤이 정해요.
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
