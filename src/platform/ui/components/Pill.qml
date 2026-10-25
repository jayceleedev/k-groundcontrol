// [0.0.2 T12 T17] specs/features/0.0.2-theme.md
/** 선택한 항목을 강조하는 단추예요.
 * 입력: text, checked와 clicked를 사용해요.
 * 출력: checked에 따라 바탕을 강조하고 clicked로 누름을 알려요.
 * 실패: 비활성 Pill은 눌러도 선택 상태가 바뀌지 않아요.
 */
import QtQuick
import Kgc.Ui

ActionButton {
    checkable: true
    kind: checked ? "primary" : "secondary"
    radius: Theme.radiusLarge
}
