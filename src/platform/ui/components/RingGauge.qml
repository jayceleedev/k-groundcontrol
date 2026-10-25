// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 진행률을 고리와 가운데 값으로 보여줘요.
 * 입력: value, from, to, unit, color를 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
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
    implicitWidth: Theme.menuHeight * 4
    implicitHeight: implicitWidth
    Shape {
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
    Column { anchors.centerIn: parent; spacing: Theme.menuGap
        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gauge.value.toLocaleString(Qt.locale(), 'f', 0); color: Theme.ink; font.pixelSize: Theme.titleSize * 2 }
        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gauge.unit; color: Theme.inkMuted; font.pixelSize: Theme.fontSize }
    }
}
