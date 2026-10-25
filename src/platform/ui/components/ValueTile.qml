// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 큰 값과 단위와 이름을 묶어 보여줘요.
 * 입력: value, unit, label을 받아요.
 * 출력: 큰 value와 작은 unit·label을 한 타일에 표시해요.
 * 실패: 빈 단위나 이름은 생략하며 값의 숫자 형식은 검사하지 않아요.
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
    implicitWidth: Math.max(Theme.valueTileWidth, contentItem.implicitWidth + padding * 2)
    implicitHeight: contentItem.implicitHeight + padding * 2
    contentItem: Column {
        spacing: Theme.valueLabelGap
        Text { text: control.value; color: Theme.ink; font.pixelSize: Theme.titleSize * 2; font.family: Theme.fontFamily }
        Text { text: control.unit + (control.unit && control.label ? " · " : "") + control.label; color: Theme.inkMuted; font.pixelSize: Theme.fontSize }
    }
    background: Rectangle { color: Theme.surfaceAlt; border.color: Theme.line; radius: Theme.radiusMedium }
}
