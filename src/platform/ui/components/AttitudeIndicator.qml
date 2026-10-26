// [0.0.4 T3 T8 T8a T8b T8c T9 T12] specs/features/0.0.4-righttabs.md
/** 기체 자세(roll·pitch)와 헤딩을 항공 자세계 관례대로 보여줘요.
 * 입력: roll·pitch(도; roll +는 오른쪽으로 기울임, pitch +는 기수 들기), heading(도), available(기체 값이 있는지, 기본 거짓)을 받아요.
 * 출력: available이 참이면 하늘·땅·수평선을 함께 화면에서 반시계로 roll도 돌리고, 돈 방향의 아래로 pitch × dialRadius / 30 픽셀 옮겨
 *       원 안에 그려요(±30°에서 원 가장자리). 아래에 헤딩을 "%1°"(0~359, 반올림)로 써요.
 *       거짓이면 수평 그림을 surfaceAlt 한 색으로 흐리게 그리고 "기체 없음"과 "---°"를 써요.
 *       dialRadius는 원 반지름(픽셀)이에요. 시험용 고정 이름은 attitudeHorizon·attitudeSkyFill·attitudeGroundFill·attitudeHeading이에요.
 * 실패: pitch는 −90~90으로 제한해 그리고, 숫자가 아닌 heading은 "---°"로 보여요.
 */
import QtQuick
import QtQuick.Effects
import QtQuick.Shapes
import Kgc.Ui

Item {
    id: indicator
    property real roll: 0
    property real pitch: 0
    property real heading: 0
    property bool available: false
    readonly property real dialRadius: Theme.attitudeDialSize / 2
    // 기체 값이 없으면 수평 그림만 남기도록, 화면에 쓰는 각도와 옮김을 여기서 한 번만 정해요.
    readonly property real shownRoll: available ? roll : 0
    readonly property real shownOffset: available ? Math.max(-90, Math.min(90, pitch)) * dialRadius / 30 : 0
    readonly property color markColor: available ? Theme.ink : Theme.inkMuted
    readonly property color symbolColor: available ? Theme.accent : Theme.inkMuted
    // 원 위쪽 가장자리의 고정 roll 눈금(0·±30·±60) 다섯 개를 바깥 끝 → 안쪽 끝 선으로 만들어요.
    readonly property var rollMarks: [0, 30, -30, 60, -60].map(angle => {
        const t = angle * Math.PI / 180
        const inner = dialRadius - Theme.attitudeMarkLength
        return [Qt.point(dialRadius + dialRadius * Math.sin(t), dialRadius - dialRadius * Math.cos(t)),
                Qt.point(dialRadius + inner * Math.sin(t), dialRadius - inner * Math.cos(t))]
    })
    // 가운데 고정 기체 표시: 양쪽 날개가 가운데로 꺾여 내려오는 모양이에요.
    readonly property var aircraftMarks: {
        const c = dialRadius, w = Theme.attitudeAircraftWidth
        return [[Qt.point(c - w / 2, c), Qt.point(c - w / 5, c), Qt.point(c - w / 10, c + w / 10)],
                [Qt.point(c + w / 2, c), Qt.point(c + w / 5, c), Qt.point(c + w / 10, c + w / 10)]]
    }
    implicitWidth: Theme.attitudeDialSize
    implicitHeight: column.implicitHeight

    // 헤딩을 0~359로 맞춘 뒤 반올림해요. 360이 되면 0이고, 숫자가 아니면 "---°"예요.
    function headingText(value: real): string {
        if (isNaN(value)) return "---°"
        const rounded = Math.round(((value % 360) + 360) % 360)
        return (rounded === 360 ? 0 : rounded) + "°"
    }

    Column {
        id: column
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: Theme.valueLabelGap
        Item {
            id: dial
            width: Theme.attitudeDialSize
            height: Theme.attitudeDialSize
            // 하늘·땅은 원 밖으로 넘치게 그리고, 둥근 가림판(dialMask)으로 원 안만 남겨요.
            Item {
                anchors.fill: parent
                layer.enabled: true
                layer.effect: MultiEffect {
                    maskEnabled: true
                    maskSource: dialMask
                    maskThresholdMin: 0.5
                    maskSpreadAtMin: 1
                }
                Item {
                    id: horizon
                    objectName: "attitudeHorizon"
                    // 옮김은 돈 항목의 아래(수평선에 수직) 방향이라, 가운데에서 그 방향으로 shownOffset만큼 자리를 옮겨요.
                    readonly property real radians: indicator.shownRoll * Math.PI / 180
                    width: dial.width
                    height: indicator.dialRadius * 8
                    x: (dial.width - width) / 2 + indicator.shownOffset * Math.sin(radians)
                    y: (dial.height - height) / 2 + indicator.shownOffset * Math.cos(radians)
                    rotation: -indicator.shownRoll
                    Rectangle {
                        objectName: "attitudeSkyFill"
                        width: parent.width
                        height: parent.height / 2
                        antialiasing: true
                        color: indicator.available ? Theme.attitudeSky : Theme.surfaceAlt
                    }
                    Rectangle {
                        objectName: "attitudeGroundFill"
                        y: parent.height / 2
                        width: parent.width
                        height: parent.height / 2
                        antialiasing: true
                        color: indicator.available ? Theme.attitudeGround : Theme.surfaceAlt
                    }
                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width
                        height: Theme.lineWidth * 2
                        antialiasing: true
                        color: indicator.markColor
                    }
                    // pitch 눈금 ±10°·±20°는 수평선과 함께 움직여요(+는 하늘 쪽).
                    Repeater {
                        model: [20, 10, -10, -20]
                        Rectangle {
                            required property int modelData
                            visible: indicator.available
                            anchors.horizontalCenter: parent.horizontalCenter
                            y: horizon.height / 2 - modelData * indicator.dialRadius / 30 - height / 2
                            width: Math.abs(modelData) === 10 ? Theme.attitudePitchMarkWide : Theme.attitudePitchMarkNarrow
                            height: Theme.lineWidth
                            antialiasing: true
                            color: Theme.ink
                        }
                    }
                }
            }
            Rectangle { id: dialMask; anchors.fill: parent; radius: width / 2; visible: false; layer.enabled: true }
            // 원 테두리, 고정 roll 눈금, 가운데 기체 표시예요.
            Shape {
                anchors.fill: parent
                preferredRendererType: Shape.CurveRenderer
                ShapePath {
                    fillColor: "transparent"
                    strokeColor: indicator.available ? Theme.lineStrong : Theme.line
                    strokeWidth: Theme.lineWidth * 2
                    PathAngleArc {
                        centerX: indicator.dialRadius; centerY: indicator.dialRadius
                        radiusX: indicator.dialRadius - Theme.lineWidth; radiusY: indicator.dialRadius - Theme.lineWidth
                        startAngle: 0; sweepAngle: 360
                    }
                }
                ShapePath {
                    fillColor: "transparent"
                    strokeColor: indicator.markColor
                    strokeWidth: Theme.lineWidth * 2
                    PathMultiline { paths: indicator.rollMarks }
                }
                ShapePath {
                    fillColor: "transparent"
                    strokeColor: indicator.symbolColor
                    strokeWidth: Theme.lineWidth * 3
                    capStyle: ShapePath.RoundCap
                    joinStyle: ShapePath.RoundJoin
                    PathMultiline { paths: indicator.aircraftMarks }
                }
            }
            // roll 지침: 수평선과 함께 돌며 위쪽 눈금을 가리키는 작은 삼각형이에요.
            Shape {
                anchors.fill: parent
                rotation: -indicator.shownRoll
                preferredRendererType: Shape.CurveRenderer
                ShapePath {
                    fillColor: indicator.symbolColor
                    strokeColor: "transparent"
                    startX: indicator.dialRadius
                    startY: Theme.attitudeMarkLength + Theme.lineWidth * 2
                    PathLine { x: indicator.dialRadius - Theme.attitudeMarkLength / 2; y: Theme.attitudeMarkLength * 2 + Theme.lineWidth * 2 }
                    PathLine { x: indicator.dialRadius + Theme.attitudeMarkLength / 2; y: Theme.attitudeMarkLength * 2 + Theme.lineWidth * 2 }
                    PathLine { x: indicator.dialRadius; y: Theme.attitudeMarkLength + Theme.lineWidth * 2 }
                }
            }
            Rectangle {
                anchors.centerIn: parent
                width: Theme.attitudeMarkLength / 2
                height: width
                radius: width / 2
                color: indicator.symbolColor
            }
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            visible: !indicator.available
            text: qsTr("기체 없음")
            color: Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            font.bold: true
        }
        Text {
            objectName: "attitudeHeading"
            anchors.horizontalCenter: parent.horizontalCenter
            text: indicator.available ? indicator.headingText(indicator.heading) : "---°"
            color: indicator.markColor
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
        }
    }
}
