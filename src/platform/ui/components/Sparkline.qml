// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 작은 공간에서 값의 추세를 보여줘요.
 * 입력: values와 color를 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import QtQuick.Shapes

Item {
    id: line
    property var values: []
    property color color: Theme.series1
    implicitWidth: Theme.languageWidth
    implicitHeight: Theme.menuHeight
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
