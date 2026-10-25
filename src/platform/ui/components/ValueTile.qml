// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 큰 값과 단위와 이름을 묶어 보여줘요.
 * 입력: value, unit, label을 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.Control {
    id: control
    property string value: ""
    property string unit: ""
    property string label: ""
    padding: Theme.inset
    implicitWidth: Math.max(Theme.menuHeight * 3, contentItem.implicitWidth + padding * 2)
    implicitHeight: contentItem.implicitHeight + padding * 2
    contentItem: Column {
        spacing: Theme.menuGap
        Text { text: control.value; color: Theme.ink; font.pixelSize: Theme.titleSize * 2; font.family: Theme.fontFamily }
        Text { text: control.unit + (control.unit && control.label ? " · " : "") + control.label; color: Theme.inkMuted; font.pixelSize: Theme.fontSize }
    }
    background: Rectangle { color: Theme.surfaceAlt; border.color: Theme.line; radius: Theme.radiusMedium }
}
