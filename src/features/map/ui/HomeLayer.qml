// [0.5.0 5.6.9 T20] specs/features/0.5.0-map.md
/** 기체마다 홈 위치를 "H" 동그라미로 그리는 지도 레이어예요.
 * 입력: 얹힌 지도 부품 mapPane과 MapController의 기체 표식 목록(homeCoordinate·hasHome)이에요.
 * 출력: 기체마다 MapQuickItem "mapHome_<sysid>" 하나예요. 가운데가 홈 좌표에 놓여요.
 * 실패: 홈이 없거나 위치 없음((0, 0)·NaN)이면 보이지 않아요.
 */
import QtQuick
import QtLocation
import Kgc.Ui
import Kgc.Map

MapItemGroup {
    property var mapPane
    MapItemView {
        model: MapController.vehicles
        delegate: MapQuickItem {
            required property var model
            objectName: "mapHome_" + model.sysid
            visible: model.hasHome
            coordinate: model.homeCoordinate
            anchorPoint.x: Theme.mapHomeSize / 2
            anchorPoint.y: Theme.mapHomeSize / 2
            sourceItem: Rectangle {
                width: Theme.mapHomeSize
                height: Theme.mapHomeSize
                radius: Theme.mapHomeSize / 2
                color: Theme.mapHome
                border.color: Theme.mapVehicleOutline
                border.width: Theme.lineWidth * 2
                Text {
                    anchors.centerIn: parent
                    text: qsTr("H")
                    font.pixelSize: Theme.captionSize
                    font.bold: true
                    color: Theme.mapHomeInk
                }
            }
        }
    }
}
