// [0.0.2 T10] specs/features/0.0.2-theme.md
/** 중립을 기준으로 양방향 값을 보여줘요.
 * 입력: value는 -100~100 사이이며 minimum과 maximum으로 관측 범위를 표시해요.
 * 출력: 중립선 양쪽으로 값 막대와 minimum·maximum 눈금을 그려요.
 * 실패: 막대와 눈금 좌표를 -100~100 안으로 제한하지만 원래 속성은 바꾸지 않아요.
 */
import QtQuick
import Kgc.Ui

Item {
    id: bar
    property real value: 0
    property real minimum: -100
    property real maximum: 100
    implicitWidth: Theme.levelBarWidth
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
        Rectangle { required property real modelData; x: (Math.max(-100, Math.min(100, modelData)) + 100) / 200 * bar.width; y: -Theme.levelTickLength; height: bar.height + Theme.levelTickLength * 2; width: Theme.lineWidth; color: Theme.lineStrong }
    }
}
