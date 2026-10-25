// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 값의 크기를 칸의 색과 범례로 보여줘요.
 * 입력: values, columns, from, to를 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Column {
    id: map
    property var values: []
    property int columns: 12
    property real from: 0
    property real to: 100
    spacing: Theme.inset
    width: Theme.languageWidth
    Grid {
        columns: Math.max(1, map.columns)
        spacing: Theme.lineWidth
        Repeater {
            model: map.values
            Rectangle {
                required property real modelData
                property real fraction: Math.max(0, Math.min(1, (modelData - map.from) / Math.max(1, map.to - map.from)))
                width: (map.width - (map.columns - 1) * Theme.lineWidth) / Math.max(1, map.columns)
                height: width
                color: fraction > 0.85 ? Theme.danger : fraction > 0.65 ? Theme.warn : Theme.series1
                opacity: Math.max(0.15, fraction)
            }
        }
    }
    Rectangle { width: parent.width; height: Theme.inset
        gradient: Gradient { orientation: Gradient.Horizontal; GradientStop { position: 0; color: Theme.surfaceAlt } GradientStop { position: 0.65; color: Theme.series1 } GradientStop { position: 0.85; color: Theme.warn } GradientStop { position: 1; color: Theme.danger } }
    }
    Item { width: parent.width; height: Theme.iconSize
        Text { text: map.from; color: Theme.inkMuted; font.pixelSize: Theme.fontSize; anchors.left: parent.left }
        Text { text: map.to; color: Theme.inkMuted; font.pixelSize: Theme.fontSize; anchors.right: parent.right }
    }
}
