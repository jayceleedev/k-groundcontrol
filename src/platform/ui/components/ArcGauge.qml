// [0.0.2 T12 T22] specs/features/0.0.2-theme.md
/** 경고 구간과 위험 구간이 있는 반원 계기예요.
 * 입력: value, from, to, warning, danger, unit을 받아요.
 * 출력: angle(value)로 제한된 바늘 각도를 구하고 경고·위험 구간과 눈금을 그려요.
 * 실패: 바늘 각도를 반원 안으로 제한해요. warning·danger의 순서는 호출자가 보장해요.
 */
import QtQuick
import Kgc.Ui
import QtQuick.Shapes

Item {
    id: gauge
    property real value: 0
    property real from: 0
    property real to: 100
    property real warning: 70
    property real danger: 90
    property string unit: ""
    readonly property real radiusValue: width / 2 - Theme.panelInset
    implicitWidth: Theme.arcGaugeWidth
    implicitHeight: Theme.arcGaugeHeight
    function angle(value: real): real { return 180 * Math.max(0, Math.min(1, (value - from) / Math.max(1, to - from))) }
    Shape {
        anchors.fill: parent
        ShapePath { fillColor: "transparent"; strokeColor: Theme.line; strokeWidth: Theme.inset
            PathAngleArc { centerX: gauge.width / 2; centerY: gauge.radiusValue + Theme.inset; radiusX: gauge.radiusValue; radiusY: gauge.radiusValue; startAngle: 180; sweepAngle: gauge.angle(gauge.warning) }
        }
        ShapePath { fillColor: "transparent"; strokeColor: Theme.warn; strokeWidth: Theme.inset
            PathAngleArc { centerX: gauge.width / 2; centerY: gauge.radiusValue + Theme.inset; radiusX: gauge.radiusValue; radiusY: gauge.radiusValue; startAngle: 180 + gauge.angle(gauge.warning); sweepAngle: gauge.angle(gauge.danger) - gauge.angle(gauge.warning) }
        }
        ShapePath { fillColor: "transparent"; strokeColor: Theme.danger; strokeWidth: Theme.inset
            PathAngleArc { centerX: gauge.width / 2; centerY: gauge.radiusValue + Theme.inset; radiusX: gauge.radiusValue; radiusY: gauge.radiusValue; startAngle: 180 + gauge.angle(gauge.danger); sweepAngle: 180 - gauge.angle(gauge.danger) }
        }
    }
    Repeater {
        model: 5
        Text {
            required property int index
            property real radians: Math.PI + index * Math.PI / 4
            text: Math.round(gauge.from + index * (gauge.to - gauge.from) / 4)
            color: Theme.inkMuted; font.pixelSize: Theme.fontSize
            x: gauge.width / 2 + Math.cos(radians) * (gauge.radiusValue - Theme.panelInset) - width / 2
            y: gauge.radiusValue + Theme.inset + Math.sin(radians) * (gauge.radiusValue - Theme.panelInset) - height / 2
        }
    }
    Rectangle {
        x: gauge.width / 2; y: gauge.radiusValue + Theme.inset - height
        width: Theme.lineWidth * 2; height: gauge.radiusValue - Theme.panelInset
        color: Theme.ink
        transformOrigin: Item.Bottom
        rotation: gauge.angle(gauge.value) - 90
    }
    Text { anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter; text: gauge.value.toLocaleString(Qt.locale(), 'f', 0) + " " + gauge.unit; color: Theme.ink; font.pixelSize: Theme.titleSize }
}
