// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 작업 단계의 진행 상태를 구분해요.
 * 입력: model은 text와 state(done, current, pending, failed)를 가진 목록이에요.
 * 출력: 각 단계의 state에 맞춘 표식과 색으로 text를 표시해요.
 * 실패: 알 수 없는 state는 남은 단계의 비활성 글자 색으로 표시해요.
 */
import QtQuick
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
