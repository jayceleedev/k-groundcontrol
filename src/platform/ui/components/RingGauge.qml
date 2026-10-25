// [0.0.2 T12 T22] specs/features/0.0.2-theme.md
// [0.0.2 T26] specs/features/0.0.2-theme.md
/** 진행률을 고리와 가운데 값으로 보여줘요.
 * 입력: value, from, to, unit, color를 받아요.
 * 출력: fraction 비율의 고리와 가운데 value·unit 글자를 그려요.
 * 실패: 고리 비율만 0~1로 제한하며 가운데 값은 받은 숫자 그대로 표시해요.
 */
import QtQuick
import Kgc.Ui
import QtQuick.Shapes

Item {
    id: gauge
    property real value: 0
    property real from: 0
    property real to: 100
    property string unit: ""
    property color color: Theme.ok
    readonly property real fraction: Math.max(0, Math.min(1, (value - from) / Math.max(1, to - from)))
    implicitWidth: Theme.ringDiameter
    implicitHeight: implicitWidth
    Shape {
        preferredRendererType: Shape.CurveRenderer
        anchors.fill: parent
        ShapePath {
            fillColor: "transparent"; strokeColor: Theme.line; strokeWidth: Theme.inset
            PathAngleArc { centerX: gauge.width / 2; centerY: gauge.height / 2; radiusX: gauge.width / 2 - Theme.inset; radiusY: gauge.height / 2 - Theme.inset; startAngle: -90; sweepAngle: 360 }
        }
        ShapePath {
            fillColor: "transparent"; strokeColor: gauge.color; strokeWidth: Theme.inset; capStyle: ShapePath.RoundCap
            PathAngleArc { centerX: gauge.width / 2; centerY: gauge.height / 2; radiusX: gauge.width / 2 - Theme.inset; radiusY: gauge.height / 2 - Theme.inset; startAngle: -90; sweepAngle: 360 * gauge.fraction }
        }
    }
    Column { anchors.centerIn: parent; spacing: Theme.valueLabelGap
        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gauge.value.toLocaleString(Qt.locale(), 'f', 0); color: Theme.ink; font.pixelSize: Theme.titleSize * 2 }
        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gauge.unit; color: Theme.inkMuted; font.pixelSize: Theme.fontSize }
    }
}
