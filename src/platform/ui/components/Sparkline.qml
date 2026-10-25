// [0.0.2 T10 T22] specs/features/0.0.2-theme.md
/** 작은 공간에서 값의 추세를 보여줘요.
 * 입력: values와 color를 받아요.
 * 출력: low·high로 값 범위를 제공하고 values 순서대로 작은 선을 그려요.
 * 실패: 빈 목록에는 선이 없어요. 유한한 숫자 목록은 호출자가 보장해요.
 */
import QtQuick
import Kgc.Ui
import QtQuick.Shapes

Item {
    id: line
    property var values: []
    property color color: Theme.series1
    implicitWidth: Theme.sparklineWidth
    implicitHeight: Theme.sparklineHeight
    readonly property real low: values.length ? Math.min(...values) : 0
    readonly property real high: values.length ? Math.max(...values) : 1
    Shape {
        anchors.fill: parent
    ShapePath {
        strokeColor: line.color
        strokeWidth: Theme.lineWidth * 2
        fillColor: "transparent"
        PathPolyline {
            path: line.values.map((value, index) => Qt.point(index * line.width / Math.max(1, line.values.length - 1),
                  line.height - Theme.lineWidth - (value - line.low) / Math.max(1, line.high - line.low) * (line.height - Theme.lineWidth * 2)))
        }
    }
    }
}
