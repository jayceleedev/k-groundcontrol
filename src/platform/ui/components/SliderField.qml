// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 단계에 맞추어 값을 조절하고 단위를 보여줘요.
 * 입력: value, from, to, stepSize, unit과 moved를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.Slider {
    id: control
    property string unit: ""
    implicitWidth: Theme.languageWidth
    implicitHeight: Theme.menuHeight
    rightPadding: Theme.menuHeight * 2
    snapMode: Basic.Slider.SnapAlways
    background: Rectangle { x: control.leftPadding; y: (control.height - height) / 2; width: control.availableWidth; height: Theme.lineWidth * 4; radius: height / 2; color: Theme.line
        Rectangle { width: parent.width * control.visualPosition; height: parent.height; color: Theme.accent; radius: parent.radius }
    }
    handle: Rectangle { x: control.leftPadding + control.visualPosition * (control.availableWidth - width); y: (control.height - height) / 2; width: Theme.iconSize; height: width; radius: height / 2; color: control.pressed ? Theme.accentPressed : Theme.surface; border.color: control.activeFocus ? Theme.focus : Theme.accent; border.width: Theme.lineWidth * 2 }
    Text { anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; text: control.value.toLocaleString(Qt.locale(), 'f', 0) + " " + control.unit; color: Theme.ink; font.pixelSize: Theme.fontSize }
}
