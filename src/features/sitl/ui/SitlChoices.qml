// [0.4.0 6.8] specs/features/0.4.0-sitl.md
/** 시뮬레이션할 기체와 배포판을 골라요.
 * 입력: SitlController.vehicle·release·editable이에요.
 * 출력: SitlController.chooseVehicle·chooseRelease 요청이에요.
 * 실패: 바꿀 수 없는 상태에서는 비활성이에요.
 * 흐름:
 * 1. SitlController.chooseVehicle — 고른 카드의 기체를 넘겨요.
 * 2. SitlController.chooseRelease — 고른 배포판을 넘겨요.
 */
import QtQuick
import Kgc.Ui
import Kgc.Sitl

Column {
    id: root
    spacing: Theme.inset
    enabled: SitlController.editable
    component SectionLabel: Text {
        color: Theme.inkMuted
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        font.bold: true
    }
    SectionLabel { text: qsTr("기체") }
    Row {
        id: vehicles
        width: parent.width
        spacing: Theme.inset
        Repeater {
            id: vehicleCards
            // 순서는 SitlVehicle 값과 같아서 index를 그대로 넘겨요.
            model: [
                {key: "copter", icon: "drone", title: qsTr("멀티콥터")},
                {key: "heli", icon: "helicopter", title: qsTr("헬리콥터")},
                {key: "plane", icon: "plane", title: qsTr("비행기")},
                {key: "rover", icon: "car", title: qsTr("로버")}
            ]
            ChoiceCard {
                required property var modelData
                required property int index
                objectName: "sitlVehicle_" + modelData.key
                width: (vehicles.width - vehicles.spacing * (vehicleCards.count - 1)) / vehicleCards.count
                icon.name: modelData.icon
                title: modelData.title
                selected: SitlController.vehicle === index
                onClicked: SitlController.chooseVehicle(index)
            }
        }
    }
    SectionLabel { text: qsTr("배포판") }
    PillGroup {
        objectName: "sitlRelease"
        // 순서는 SitlRelease 값과 같아요.
        model: [qsTr("안정판"), qsTr("베타"), qsTr("최신 개발판")]
        currentIndex: SitlController.release
        onActivated: index => SitlController.chooseRelease(index)
    }
}
