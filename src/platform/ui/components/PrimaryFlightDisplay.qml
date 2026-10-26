// [0.0.4 T3 T19 T20 T21 T23 T23a] specs/features/0.0.4-righttabs.md
/** 칸 전체를 하늘·땅으로 채운 PFD로 자세·속도·고도·헤딩과 OSD를 보여줘요.
 * 입력: roll·pitch(도; roll +는 오른쪽으로 기울임, pitch +는 기수 들기), heading(도), speed(m/s), altitude(m), available(기본 거짓)과
 *       OSD 입력 flightMode·armed·batteryVoltage·batteryPercent·gpsFix·satellites·homeDistance·climbRate·linkQuality를 받아요.
 * 출력: 하늘·땅·수평선·피치 사다리(attitudeHorizon)를 화면에서 반시계로 roll도 돌리고 pitch × 높이 / 60 픽셀 아래로 옮겨요(±30°에서 위·아래 가장자리).
 *       위쪽 roll 호와 지침, 가운데 노란 고정 기체 표시, 왼쪽 속도 띠(speedReadout, 소수 한 자리)·오른쪽 고도 띠(altitudeReadout, 정수),
 *       아래 방위 띠(attitudeHeading, "%1°")를 그리고, 그 위에 OSD 여섯 글자를 얹어요.
 * 실패: pitch는 −90~90으로 제한하고, 숫자가 아닌 속도·고도는 "—", 헤딩은 "---°"예요.
 *       available이 거짓이면 수평으로 두고 속도·고도 "—", 방위 "---°", 가운데 "기체 없음", OSD는 모두 "—"예요.
 */
import QtQuick
import QtQuick.Shapes
import Kgc.Ui

Item {
    id: pfd
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
    // 기체 값이 없으면 수평 그림만 남기도록, 화면에 쓰는 각도와 옮김을 여기서 한 번만 정해요.
    readonly property real shownRoll: available ? roll : 0
    readonly property real shownOffset: available ? Math.max(-90, Math.min(90, pitch)) * height / 60 : 0
    readonly property real centerX: width / 2
    readonly property real centerY: height / 2
    // roll 호는 위 OSD 줄 아래에서 시작해요.
    readonly property real arcRadius: Math.max(0, centerY - osd.topReserve - Theme.lineWidth * 2)
    // roll 지침 삼각형의 꼭짓점 y예요(호 안쪽, 긴 눈금 바로 아래).
    readonly property real pointerTip: centerY - arcRadius + Theme.attitudeMarkLength + Theme.lineWidth
    implicitWidth: Theme.rightBarWidth - Theme.inset * 2
    implicitHeight: Theme.instrumentHeight - Theme.instrumentStyleBarHeight
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
        readonly property real radians: pfd.shownRoll * Math.PI / 180
        width: (pfd.width + pfd.height) * 2
        height: pfd.height * 8
        x: (pfd.width - width) / 2 + pfd.shownOffset * Math.sin(radians)
        y: (pfd.height - height) / 2 + pfd.shownOffset * Math.cos(radians)
        rotation: -pfd.shownRoll
        Rectangle { objectName: "attitudeSkyFill"; width: parent.width; height: parent.height / 2; color: Theme.attitudeSky }
        Rectangle { objectName: "attitudeGroundFill"; y: parent.height / 2; width: parent.width; height: parent.height / 2; color: Theme.attitudeGround }
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            height: Theme.lineWidth * 2
            antialiasing: true
            color: Theme.attitudeMark
        }
        // 피치 사다리 ±10°(긴 것)·±20°(짧은 것)는 수평선과 함께 움직여요(+는 하늘 쪽).
        Repeater {
            model: [20, 10, -10, -20]
            Rectangle {
                required property int modelData
                anchors.horizontalCenter: parent.horizontalCenter
                y: horizon.height / 2 - modelData * pfd.height / 60 - height / 2
                width: Math.abs(modelData) === 10 ? Theme.attitudePitchMarkWide : Theme.attitudePitchMarkNarrow
                height: Theme.lineWidth * 2
                antialiasing: true
                color: Theme.attitudeMark
            }
        }
    }
    // 위쪽 roll 호(±60°)와 0·±10·±20·±30·±45·±60 눈금이에요. 화면에 고정이에요.
    Shape {
        anchors.fill: parent
        preferredRendererType: Shape.CurveRenderer
        ShapePath {
            fillColor: "transparent"
            strokeColor: Theme.attitudeMark
            strokeWidth: Theme.lineWidth * 2
            PathAngleArc {
                centerX: pfd.centerX; centerY: pfd.centerY
                radiusX: pfd.arcRadius; radiusY: pfd.arcRadius
                startAngle: -150; sweepAngle: 120
            }
        }
        ShapePath {
            fillColor: "transparent"
            strokeColor: Theme.attitudeMark
            strokeWidth: Theme.lineWidth * 2
            PathMultiline {
                paths: [0, 10, -10, 20, -20, 30, -30, 45, -45, 60, -60].map(angle => {
                    const t = angle * Math.PI / 180
                    const outer = pfd.arcRadius
                    const inner = outer - (angle % 30 === 0 ? Theme.attitudeMarkLength : Theme.attitudeMarkLength / 2)
                    return [Qt.point(pfd.centerX + outer * Math.sin(t), pfd.centerY - outer * Math.cos(t)),
                            Qt.point(pfd.centerX + inner * Math.sin(t), pfd.centerY - inner * Math.cos(t))]
                })
            }
        }
    }
    // roll 지침: 수평선과 함께 돌며 호 안쪽에서 눈금을 가리키는 작은 삼각형이에요.
    Shape {
        anchors.fill: parent
        rotation: -pfd.shownRoll
        preferredRendererType: Shape.CurveRenderer
        ShapePath {
            fillColor: Theme.attitudeMark
            strokeColor: "transparent"
            startX: pfd.centerX; startY: pfd.pointerTip
            PathLine { x: pfd.centerX - Theme.attitudeMarkLength / 2; y: pfd.pointerTip + Theme.attitudeMarkLength }
            PathLine { x: pfd.centerX + Theme.attitudeMarkLength / 2; y: pfd.pointerTip + Theme.attitudeMarkLength }
            PathLine { x: pfd.centerX; y: pfd.pointerTip }
        }
    }
    // 가운데 노란 고정 기체 표시: 양쪽 날개가 가운데로 꺾여 내려오는 모양과 가운데 점이에요.
    Shape {
        anchors.fill: parent
        preferredRendererType: Shape.CurveRenderer
        ShapePath {
            fillColor: "transparent"
            strokeColor: Theme.attitudeAircraft
            strokeWidth: Theme.lineWidth * 3
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin
            PathMultiline {
                paths: {
                    const c = pfd.centerX, m = pfd.centerY, w = Theme.attitudeAircraftWidth
                    return [[Qt.point(c - w / 2, m), Qt.point(c - w / 5, m), Qt.point(c - w / 10, m + w / 10)],
                            [Qt.point(c + w / 2, m), Qt.point(c + w / 5, m), Qt.point(c + w / 10, m + w / 10)]]
                }
            }
        }
    }
    Rectangle {
        x: pfd.centerX - width / 2
        y: pfd.centerY - height / 2
        width: Theme.attitudeMarkLength / 2
        height: width
        radius: width / 2
        color: Theme.attitudeAircraft
    }

    // 속도·고도 띠: 어두운 반투명 띠 위에 값과 함께 흐르는 눈금, 가운데 숫자 상자예요.
    component ValueTape: Rectangle {
        id: tape
        property string readoutName
        property real value: NaN
        property int digits: 0
        property real step: 1
        property real span: 10
        property bool leftSide: true
        readonly property bool live: pfd.available && isFinite(value)
        readonly property real pixelsPerUnit: height / span
        y: osd.topReserve
        width: Theme.attitudeTapeWidth
        height: Math.max(0, pfd.height - osd.topReserve - osd.bottomReserve)
        color: Theme.attitudeTape
        clip: true
        Repeater {
            model: tape.live ? Math.ceil(tape.span / tape.step) + 2 : 0
            Rectangle {
                required property int index
                readonly property real mark: (Math.floor(tape.value / tape.step) + index - Math.floor(tape.span / tape.step / 2)) * tape.step
                x: tape.leftSide ? tape.width - width : 0
                y: tape.height / 2 - (mark - tape.value) * tape.pixelsPerUnit - height / 2
                width: Theme.attitudeMarkLength
                height: Theme.lineWidth * 2
                color: Theme.attitudeMark
            }
        }
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            height: Theme.attitudeReadoutHeight
            color: Theme.osdShadow
            border.color: Theme.attitudeMark
            border.width: Theme.lineWidth
            Text {
                objectName: tape.readoutName
                anchors.centerIn: parent
                text: pfd.valueText(tape.value, tape.digits)
                color: Theme.osdInk
                style: Text.Outline
                styleColor: Theme.osdShadow
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
            }
        }
    }
    ValueTape { readoutName: "speedReadout"; value: pfd.speed; digits: 1; step: 2; span: 20; leftSide: true }
    ValueTape { readoutName: "altitudeReadout"; value: pfd.altitude; step: 10; span: 100; leftSide: false; x: pfd.width - width }

    // 아래 방위 띠: 10°마다 눈금이 헤딩과 함께 흐르고 가운데에 현재 헤딩이 있어요.
    Rectangle {
        id: headingTape
        readonly property real pixelsPerDegree: width / 60
        y: pfd.height - height
        width: pfd.width
        height: Theme.attitudeHeadingTapeHeight
        color: Theme.attitudeTape
        clip: true
        Repeater {
            model: pfd.available && isFinite(pfd.heading) ? 9 : 0
            Rectangle {
                required property int index
                readonly property real mark: (Math.floor(pfd.heading / 10) + index - 4) * 10
                x: headingTape.width / 2 + (mark - pfd.heading) * headingTape.pixelsPerDegree - width / 2
                width: Theme.lineWidth * 2
                height: mark % 30 === 0 ? Theme.attitudeMarkLength : Theme.attitudeMarkLength / 2
                color: Theme.attitudeMark
            }
        }
        Rectangle {
            anchors.centerIn: parent
            width: headingLabel.implicitWidth + Theme.inset
            height: parent.height
            color: Theme.osdShadow
            Text {
                id: headingLabel
                objectName: "attitudeHeading"
                anchors.centerIn: parent
                text: pfd.headingText(pfd.heading)
                color: Theme.osdInk
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
            }
        }
    }
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        y: pfd.centerY + Theme.attitudeAircraftWidth / 5
        visible: !pfd.available
        text: qsTr("기체 없음")
        color: Theme.osdInk
        style: Text.Outline
        styleColor: Theme.osdShadow
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        font.bold: true
    }
    InstrumentOsd {
        id: osd
        anchors.fill: parent
        available: pfd.available
        bottomInset: Theme.attitudeHeadingTapeHeight
        inkColor: Theme.osdInk
        outlined: true
    }
}
