// [0.0.4 T3] specs/features/0.0.4-righttabs.md
// [0.0.4 T18 T20 T24d T27 T29] specs/features/0.0.4-righttabs.md
// 우 사이드바 계기 칸: 맨 위 줄의 단추 넷으로 계기 모양(ui.instrumentStyle)을 고르고, 그 아래 그림 칸에 고른 모양을 실어요.
// 기체 값은 아직 없어서(0.1.x부터) 넷 모두 "기체 없음"(available: false)으로 그려요.
import QtQuick
import Kgc.Ui
Item {
    id: view
    // 단추 아이콘과 이름 힌트예요. 단추 순서는 uiSettingValues.instrumentStyles(설정 메타데이터) 순서를 따라요.
    readonly property var icons: ({ round: "gauge", pfd: "device-desktop-analytics", hud: "focus-2", model3d: "cube-3d-sphere" })
    readonly property var hints: ({ round: qsTr("둥근 계기"), pfd: qsTr("PFD"), hud: qsTr("HUD"), model3d: qsTr("3D") })
    readonly property var pictures: ({ round: roundPicture, pfd: pfdPicture, hud: hudPicture, model3d: model3dPicture })

    Row {
        id: styleBar
        objectName: "instrumentStyleBar"
        anchors.right: parent.right
        height: Theme.instrumentStyleBarHeight
        Accessible.name: qsTr("계기 모양")
        Repeater {
            model: uiSettingValues.instrumentStyles
            IconButton {
                required property string modelData
                objectName: "instrumentStyle_" + modelData
                flat: true
                width: Theme.instrumentStyleBarHeight
                height: Theme.instrumentStyleBarHeight
                name: view.icons[modelData] ?? ""
                hint: view.hints[modelData] ?? modelData
                checked: uiSettingValues.instrumentStyle === modelData
                onClicked: uiSettingValues.instrumentStyle = modelData
            }
        }
    }
    // 그림 칸 = 계기 칸 안쪽 전체에서 단추 줄 높이를 뺀 것이에요.
    Loader {
        id: picture
        objectName: "instrumentPicture"
        anchors.top: styleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        sourceComponent: view.pictures[uiSettingValues.instrumentStyle] ?? pfdPicture
    }
    Component {
        id: roundPicture
        Item {
            // 둥근 계기 지름은 min(attitudeDialSize, 그림 칸 높이 − 글자 두 줄 높이)예요.
            FontMetrics { id: line; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSize }
            AttitudeIndicator {
                anchors.centerIn: parent
                available: false
                maxDialSize: parent.height - (line.height + Theme.valueLabelGap) * 2
            }
        }
    }
    Component { id: pfdPicture; PrimaryFlightDisplay { available: false } }
    Component { id: hudPicture; HeadUpDisplay { available: false } }
    Component { id: model3dPicture; VehicleAttitude3D { available: false } }
}
