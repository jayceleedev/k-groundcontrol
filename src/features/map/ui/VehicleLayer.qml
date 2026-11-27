// [0.5.0 5.6.9 T18 T24] specs/features/0.5.0-map.md
/** 기체마다 기수 방향으로 돌아가는 화살표를 그리는 지도 레이어예요.
 * 입력: 얹힌 지도 부품 mapPane과 MapController의 기체 표식 목록(coordinate·heading·active·hasPosition)이에요.
 * 출력: 기체마다 MapQuickItem "mapVehicle_<sysid>" 하나예요. 활성 기체는 mapVehicle, 다른 기체는 mapVehicleOther 색이에요.
 * 실패: 위치 없음((0, 0)·NaN)이면 보이지 않아요.
 */
import QtQuick
import QtQuick.Shapes
import QtLocation
import Kgc.Ui
import Kgc.Map

MapItemGroup {
    property var mapPane
    MapItemView {
        model: MapController.vehicles
        delegate: MapQuickItem {
            id: marker
            required property var model
            objectName: "mapVehicle_" + model.sysid
            visible: model.hasPosition
            coordinate: model.coordinate
            anchorPoint.x: Theme.mapVehicleSize / 2
            anchorPoint.y: Theme.mapVehicleSize / 2
            sourceItem: Item {
                width: Theme.mapVehicleSize
                height: Theme.mapVehicleSize
                // MapQuickItem이 sourceItem의 회전 기준을 왼쪽 위로 바꿔 놓으므로, 안쪽 Shape를 가운데 기준으로 돌려요.
                Shape {
                    anchors.fill: parent
                    rotation: marker.model.heading
                    preferredRendererType: Shape.CurveRenderer
                    ShapePath {
                        fillColor: marker.model.active ? Theme.mapVehicle : Theme.mapVehicleOther
                        strokeColor: Theme.mapVehicleOutline
                        strokeWidth: Theme.lineWidth * 2
                        joinStyle: ShapePath.RoundJoin
                        startX: Theme.mapVehicleSize * 0.5
                        startY: 0
                        PathLine { x: Theme.mapVehicleSize * 0.9; y: Theme.mapVehicleSize * 0.9 }
                        PathLine { x: Theme.mapVehicleSize * 0.5; y: Theme.mapVehicleSize * 0.7 }
                        PathLine { x: Theme.mapVehicleSize * 0.1; y: Theme.mapVehicleSize * 0.9 }
                        PathLine { x: Theme.mapVehicleSize * 0.5; y: 0 }
                    }
                }
            }
        }
    }
}
