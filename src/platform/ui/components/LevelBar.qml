// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 중립을 기준으로 양방향 값을 보여줘요.
 * 입력: value는 -100~100 사이이며 minimum과 maximum으로 관측 범위를 표시해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Item {
    id: bar
    property real value: 0
    property real minimum: -100
    property real maximum: 100
    implicitWidth: Theme.languageWidth
    implicitHeight: Theme.iconSize
    Rectangle { anchors.fill: parent; color: Theme.surfaceAlt; radius: Theme.radiusSmall; border.color: Theme.line }
    Rectangle {
        x: Math.min(parent.width / 2, parent.width * (Math.max(-100, Math.min(100, bar.value)) + 100) / 200)
        width: Math.abs(Math.max(-100, Math.min(100, bar.value))) / 200 * parent.width
        height: parent.height
        color: Theme.accent
        radius: Theme.radiusSmall
    }
    Repeater {
        model: [bar.minimum, 0, bar.maximum]
        Rectangle { required property real modelData; x: (Math.max(-100, Math.min(100, modelData)) + 100) / 200 * bar.width; y: -Theme.menuGap; height: bar.height + Theme.menuGap * 2; width: Theme.lineWidth; color: Theme.lineStrong }
    }
}
