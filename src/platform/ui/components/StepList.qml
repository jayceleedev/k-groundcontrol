// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 작업 단계의 진행 상태를 구분해요.
 * 입력: model은 text와 state(done, current, pending, failed)를 가진 목록이에요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Column {
    id: control
    property var model: []
    spacing: Theme.inset
    Repeater {
        model: control.model
        Text {
            required property var modelData
            text: (modelData.state === "done" ? "✓ " : modelData.state === "failed" ? "× " : "● ") + modelData.text
            color: modelData.state === "done" ? Theme.ok : modelData.state === "current" ? Theme.accent : modelData.state === "failed" ? Theme.danger : Theme.inkDisabled
            font.pixelSize: Theme.fontSize
        }
    }
}
