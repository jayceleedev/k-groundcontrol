// [0.0.4 T20 T24a T24b T24c T24d] specs/features/0.0.4-righttabs.md
/** 기체 모형(쿼드콥터 또는 비행기)이 자세대로 도는 3D 계기예요.
 * 입력: available(기본 거짓), roll·pitch·heading(도; roll +는 오른쪽 날개 내려감, pitch +는 기수 듦, heading은 위에서 본 시계 방향 기수 방위),
 *       modelKind("quad"|"plane"), hasCompass(나침반이 있는 기체인지)를 받아요.
 * 출력: 기체 뒤 위에서 보는 고정 카메라로 모형 묶음(vehicleModel)을 eulerRotation (pitch, −heading, −roll)으로 돌려 그려요.
 *       heading 0이면 기수가 화면 안쪽이에요. hasCompass가 참이면 모형 아래 나침반 고리와 N 표식(compassNorth)을 장면에 고정해 그려
 *       모형만 돌고 자북(화면 안쪽)은 그대로예요. 아래에 헤딩(attitudeHeading, "%1°")을 써요.
 * 실패: available이 거짓이면 정자세·heading 0·쿼드로 두고 고리·N을 그리지 않으며 "기체 없음"과 "---°"를 써요.
 *       모르는 modelKind는 쿼드로 그려요.
 */
import QtQuick
import QtQuick3D
import Kgc.Ui

Item {
    id: root
    property bool available: false
    property real roll: 0
    property real pitch: 0
    property real heading: 0
    property string modelKind: "quad"
    property bool hasCompass: false
    readonly property real shownRoll: available ? roll : 0
    readonly property real shownPitch: available ? Math.max(-90, Math.min(90, pitch)) : 0
    readonly property real shownHeading: available ? heading : 0
    readonly property bool showPlane: available && modelKind === "plane"
    readonly property bool showCompass: available && hasCompass
    readonly property string northLetter: "N"
    // 장면 단위예요(#Cube·#Cylinder·#Cone 기본 도형의 한 변이 100). 나침반 고리는 모형보다 조금 아래에 깔아요.
    readonly property real compassRadius: 180
    readonly property real compassLevel: -70
    implicitWidth: Theme.rightBarWidth - Theme.inset * 2
    implicitHeight: Theme.instrumentHeight - Theme.instrumentStyleBarHeight

    // 헤딩을 0~359로 맞춘 뒤 반올림해요. 360이 되면 0이고, 숫자가 아니면 "---°"예요.
    function headingText(value: real): string {
        if (!available || isNaN(value)) return "---°"
        const rounded = Math.round(((value % 360) + 360) % 360)
        return (rounded === 360 ? 0 : rounded) + "°"
    }

    component Part: Model {
        property color tint: Theme.modelBody
        source: "#Cube"
        materials: PrincipledMaterial { baseColor: tint; roughness: 0.6 }
    }

    View3D {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: labels.top
        environment: SceneEnvironment {
            backgroundMode: SceneEnvironment.Color
            clearColor: Theme.surface
            antialiasingMode: SceneEnvironment.MSAA
            antialiasingQuality: SceneEnvironment.High
        }
        // 기체 뒤(+z) 위에서 원점을 내려다보는 고정 카메라예요. 날개 끝과 나침반 고리가 칸 안에 들도록 거리를 잡았어요.
        PerspectiveCamera {
            position: Qt.vector3d(0, 300, 430)
            eulerRotation.x: -Math.atan2(300, 430) * 180 / Math.PI
            fieldOfView: 50
        }
        DirectionalLight { eulerRotation: Qt.vector3d(-50, -30, 0); brightness: 1.2 }
        DirectionalLight { eulerRotation: Qt.vector3d(-20, 150, 0); brightness: 0.5 }

        // 나침반 고리와 N은 장면에 고정이라 모형이 돌아도 자리가 바뀌지 않아요(자북 = 화면 안쪽 −z).
        Node {
            visible: root.showCompass
            y: root.compassLevel
            Repeater3D {
                model: 48
                Part {
                    required property int index
                    readonly property real angle: index * 2 * Math.PI / 48
                    tint: Theme.compassRing
                    position: Qt.vector3d(root.compassRadius * Math.sin(angle), 0, -root.compassRadius * Math.cos(angle))
                    eulerRotation.y: -index * 360 / 48
                    scale: Qt.vector3d(0.2, 0.03, 0.06)
                }
            }
            Node {
                id: compassNorth
                objectName: "compassNorth"
                // 3D 노드의 visible은 부모를 따라 바뀌지 않아서, N 표식에도 같은 조건을 직접 걸어요.
                visible: root.showCompass
                position: Qt.vector3d(0, 0, -root.compassRadius - 30)
                Part { source: "#Cone"; tint: Theme.compassNorthColor; eulerRotation.x: -90; scale: Qt.vector3d(0.25, 0.3, 0.25); z: 20 }
                Node {
                    y: 40
                    Text {
                        anchors.centerIn: parent
                        text: root.northLetter
                        color: Theme.compassNorthColor
                        font.family: Theme.fontFamily
                        font.pixelSize: 48
                        font.bold: true
                    }
                }
            }
        }

        Node {
            id: vehicleModel
            objectName: "vehicleModel"
            eulerRotation: Qt.vector3d(root.shownPitch, -root.shownHeading, -root.shownRoll)
            // 쿼드: 가운데 몸체, X자 팔 둘, 팔 끝 프롭 넷(앞 둘은 기수 색), 몸체 앞 기수 표시예요.
            Node {
                objectName: "modelQuad"
                visible: !root.showPlane
                Part { scale: Qt.vector3d(0.6, 0.18, 0.8) }
                Part { tint: Theme.modelAccent; position: Qt.vector3d(0, 5, -45); scale: Qt.vector3d(0.3, 0.12, 0.15) }
                Part { eulerRotation.y: 45; scale: Qt.vector3d(2.4, 0.07, 0.12) }
                Part { eulerRotation.y: -45; scale: Qt.vector3d(2.4, 0.07, 0.12) }
                Repeater3D {
                    model: [Qt.point(-1, -1), Qt.point(1, -1), Qt.point(-1, 1), Qt.point(1, 1)]
                    Part {
                        required property point modelData
                        source: "#Cylinder"
                        tint: modelData.y < 0 ? Theme.modelAccent : Theme.modelBody
                        position: Qt.vector3d(modelData.x * 85, 12, modelData.y * 85)
                        scale: Qt.vector3d(0.75, 0.03, 0.75)
                    }
                }
            }
            // 비행기: 동체, 주날개, 꼬리날개, 수직 꼬리, 기수 원뿔(기수 색)이에요.
            Node {
                objectName: "modelPlane"
                visible: root.showPlane
                Part { scale: Qt.vector3d(0.24, 0.24, 2.2) }
                Part { position: Qt.vector3d(0, 4, -20); scale: Qt.vector3d(3.0, 0.04, 0.55) }
                Part { position: Qt.vector3d(0, 4, 95); scale: Qt.vector3d(1.1, 0.04, 0.3) }
                Part { position: Qt.vector3d(0, 30, 95); scale: Qt.vector3d(0.04, 0.5, 0.3) }
                Part { source: "#Cone"; tint: Theme.modelAccent; position: Qt.vector3d(0, 0, -110); eulerRotation.x: -90; scale: Qt.vector3d(0.24, 0.3, 0.24) }
            }
        }
    }

    Column {
        id: labels
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        bottomPadding: Theme.valueLabelGap
        spacing: Theme.valueLabelGap
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            visible: !root.available
            text: qsTr("기체 없음")
            color: Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            font.bold: true
        }
        Text {
            objectName: "attitudeHeading"
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.headingText(root.heading)
            color: root.available ? Theme.ink : Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
        }
    }
}
