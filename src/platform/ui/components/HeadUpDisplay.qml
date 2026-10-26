// [0.0.4 T19 T20 T21 T23 T23a] specs/features/0.0.4-righttabs.md
/** 어두운 바탕 위에 초록 선과 글자만으로 자세·속도·고도·헤딩과 OSD를 보여주는 HUD예요.
 * 입력: roll·pitch(도; roll +는 오른쪽으로 기울임, pitch +는 기수 들기), heading(도), speed(m/s), altitude(m), available(기본 거짓)과
 *       OSD 입력 flightMode·armed·batteryVoltage·batteryPercent·gpsFix·satellites·homeDistance·climbRate·linkQuality를 받아요.
 * 출력: 수평선과 피치 사다리(attitudeHorizon; 아래쪽은 점선)를 PFD와 같은 식으로 반시계 roll도 돌리고 pitch × 높이 / 60 픽셀 옮겨요.
 *       가운데 비행경로 표식(이번엔 늘 가운데), 위 방위 띠(attitudeHeading), 왼쪽 속도(speedReadout, 소수 한 자리)·
 *       오른쪽 고도(altitudeReadout, 정수) 숫자, OSD 여섯 글자를 모두 hudInk로 그려요.
 * 실패: pitch는 −90~90으로 제한하고, 숫자가 아닌 속도·고도는 "—", 헤딩은 "---°"예요.
 *       available이 거짓이면 수평으로 두고 "—"·"---°"와 가운데 "기체 없음"을 쓰며 OSD는 모두 "—"예요.
 */
import QtQuick
import QtQuick.Shapes
import Kgc.Ui

Rectangle {
    id: hud
    property real roll: 0
    property real pitch: 0
    property real heading: 0
    property real speed: NaN
    property real altitude: NaN
    property bool available: false
    property alias flightMode: osd.flightMode
    property alias armed: osd.armed
    property alias batteryVoltage: osd.batteryVoltage
    property alias batteryPercent: osd.batteryPercent
    property alias gpsFix: osd.gpsFix
    property alias satellites: osd.satellites
    property alias homeDistance: osd.homeDistance
    property alias climbRate: osd.climbRate
    property alias linkQuality: osd.linkQuality
    readonly property real shownRoll: available ? roll : 0
    readonly property real shownOffset: available ? Math.max(-90, Math.min(90, pitch)) * height / 60 : 0
    readonly property real centerX: width / 2
    readonly property real centerY: height / 2
    readonly property real markerRadius: Theme.attitudeMarkLength * 3 / 4
    implicitWidth: Theme.rightBarWidth - Theme.inset * 2
    implicitHeight: Theme.instrumentHeight - Theme.instrumentStyleBarHeight
    color: Theme.hudBackground
    clip: true

    // 헤딩을 0~359로 맞춘 뒤 반올림해요. 360이 되면 0이고, 숫자가 아니면 "---°"예요.
    function headingText(value: real): string {
        if (!available || isNaN(value)) return "---°"
        const rounded = Math.round(((value % 360) + 360) % 360)
        return (rounded === 360 ? 0 : rounded) + "°"
    }
    function valueText(value: real, digits: int): string {
        if (!available || !isFinite(value)) return "—"
        return digits > 0 ? value.toFixed(digits) : String(Math.round(value))
    }

    Item {
        id: horizon
        objectName: "attitudeHorizon"
        // 옮김은 돈 항목의 아래(수평선에 수직) 방향이라, 가운데에서 그 방향으로 shownOffset만큼 자리를 옮겨요.
        readonly property real radians: hud.shownRoll * Math.PI / 180
        readonly property real middle: height / 2
        width: (hud.width + hud.height) * 2
        height: hud.height * 8
        x: (hud.width - width) / 2 + hud.shownOffset * Math.sin(radians)
        y: (hud.height - height) / 2 + hud.shownOffset * Math.cos(radians)
        rotation: -hud.shownRoll
        // 수평선은 가운데 비행경로 표식 둘레만 비워 두고, 사다리는 위쪽(+)은 실선, 아래쪽(−)은 점선이에요.
        Shape {
            anchors.fill: parent
            preferredRendererType: Shape.CurveRenderer
            ShapePath {
                fillColor: "transparent"
                strokeColor: Theme.hudInk
                strokeWidth: Theme.lineWidth * 2
                PathMultiline {
                    paths: {
                        const c = horizon.width / 2, m = horizon.middle, gap = Theme.attitudeAircraftWidth / 2
                        const lines = [[Qt.point(0, m), Qt.point(c - gap, m)], [Qt.point(c + gap, m), Qt.point(horizon.width, m)]]
                        for (const degree of [10, 20]) {
                            const y = m - degree * hud.height / 60
                            const half = (degree === 10 ? Theme.attitudePitchMarkWide : Theme.attitudePitchMarkNarrow) / 2
                            lines.push([Qt.point(c - half, y), Qt.point(c + half, y)])
                        }
                        return lines
                    }
                }
            }
            ShapePath {
                fillColor: "transparent"
                strokeColor: Theme.hudInk
                strokeWidth: Theme.lineWidth * 2
                strokeStyle: ShapePath.DashLine
                dashPattern: [2, 2]
                PathMultiline {
                    paths: [10, 20].map(degree => {
                        const c = horizon.width / 2, y = horizon.middle + degree * hud.height / 60
                        const half = (degree === 10 ? Theme.attitudePitchMarkWide : Theme.attitudePitchMarkNarrow) / 2
                        return [Qt.point(c - half, y), Qt.point(c + half, y)]
                    })
                }
            }
        }
    }
    // 비행경로 표식: 작은 원과 양 날개, 위쪽 꼬리예요. 기체 모델이 없는 이번 버전엔 늘 가운데에 있어요.
    Shape {
        anchors.fill: parent
        preferredRendererType: Shape.CurveRenderer
        ShapePath {
            fillColor: "transparent"
            strokeColor: Theme.hudInk
            strokeWidth: Theme.lineWidth * 2
            PathAngleArc {
                centerX: hud.centerX; centerY: hud.centerY
                radiusX: hud.markerRadius; radiusY: hud.markerRadius
                startAngle: 0; sweepAngle: 360
            }
        }
        ShapePath {
            fillColor: "transparent"
            strokeColor: Theme.hudInk
            strokeWidth: Theme.lineWidth * 2
            PathMultiline {
                paths: {
                    const c = hud.centerX, m = hud.centerY, r = hud.markerRadius, wing = Theme.attitudeMarkLength * 2
                    return [[Qt.point(c - r - wing, m), Qt.point(c - r, m)], [Qt.point(c + r, m), Qt.point(c + r + wing, m)],
                            [Qt.point(c, m - r), Qt.point(c, m - r - Theme.attitudeMarkLength)]]
                }
            }
        }
    }

    component HudText: Text {
        color: Theme.hudInk
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
    }
    // 위 방위 띠: 위 OSD 줄 바로 아래에 10°마다 눈금이 흐르고 가운데 상자에 현재 헤딩이 있어요.
    Item {
        id: headingTape
        readonly property real pixelsPerDegree: width / 60
        y: osd.topReserve
        width: hud.width
        height: Theme.attitudeHeadingTapeHeight
        clip: true
        Repeater {
            model: hud.available && isFinite(hud.heading) ? 9 : 0
            Rectangle {
                required property int index
                readonly property real mark: (Math.floor(hud.heading / 10) + index - 4) * 10
                x: headingTape.width / 2 + (mark - hud.heading) * headingTape.pixelsPerDegree - width / 2
                y: headingTape.height - height
                width: Theme.lineWidth * 2
                height: mark % 30 === 0 ? Theme.attitudeMarkLength : Theme.attitudeMarkLength / 2
                color: Theme.hudInk
            }
        }
        Rectangle {
            anchors.centerIn: parent
            width: headingLabel.implicitWidth + Theme.inset
            height: parent.height
            color: Theme.hudBackground
            border.color: Theme.hudInk
            border.width: Theme.lineWidth
            HudText { id: headingLabel; objectName: "attitudeHeading"; anchors.centerIn: parent; text: hud.headingText(hud.heading) }
        }
    }
    // 왼쪽 속도·오른쪽 고도 숫자 상자예요. 띠 없이 숫자만 둬요.
    component Readout: Rectangle {
        id: box
        property string readoutName
        property real value: NaN
        property int digits: 0
        y: hud.centerY - height / 2
        width: Theme.attitudeTapeWidth
        height: Theme.attitudeReadoutHeight
        color: Theme.hudBackground
        border.color: Theme.hudInk
        border.width: Theme.lineWidth
        HudText { objectName: box.readoutName; anchors.centerIn: parent; text: hud.valueText(box.value, box.digits) }
    }
    Readout { readoutName: "speedReadout"; value: hud.speed; digits: 1; x: Theme.inset / 2 }
    Readout { readoutName: "altitudeReadout"; value: hud.altitude; x: hud.width - width - Theme.inset / 2 }
    HudText {
        anchors.horizontalCenter: parent.horizontalCenter
        y: hud.centerY + hud.markerRadius + Theme.attitudeMarkLength
        visible: !hud.available
        text: qsTr("기체 없음")
        font.bold: true
    }
    InstrumentOsd {
        id: osd
        anchors.fill: parent
        available: hud.available
        bottomInset: 0
        inkColor: Theme.hudInk
        outlined: false
    }
}
