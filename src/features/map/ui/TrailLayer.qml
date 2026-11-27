// [0.5.0 5.6.9 T19 T27] specs/features/0.5.0-map.md
/** 기체마다 지나온 길(궤적)을 선으로 그리는 지도 레이어예요.
 * 입력: 얹힌 지도 부품 mapPane과 MapController의 기체 표식 목록·궤적이에요.
 * 출력: 기체마다 MapPolyline "mapTrail_<sysid>" 하나예요. 만들 때 그 기체의 궤적을 받고, 점이 더해지면 끝에 붙여요.
 * 실패: 궤적 최대 점 수를 넘는 점은 오래된 쪽부터 지워요. 궤적 지우기에는 선을 비워요.
 */
import QtQuick
import QtLocation
import Kgc.Ui
import Kgc.Map

MapItemGroup {
    property var mapPane
    // Repeater는 지도에 얹히지 않으므로 MapItemView로 기체마다 선을 만들어요.
    MapItemView {
        model: MapController.vehicles
        delegate: MapPolyline {
            id: trail
            required property var model
            readonly property int sysid: model.sysid
            objectName: "mapTrail_" + sysid
            line.color: Theme.mapTrail
            line.width: Theme.mapTrailWidth
            Component.onCompleted: path = MapController.trailOf(sysid)
            Connections {
                target: MapController
                function onTrailPointAdded(sysid, coordinate) {
                    if (sysid !== trail.sysid) return
                    trail.addCoordinate(coordinate)
                    while (trail.pathLength() > MapController.trailMaxPoints) trail.removeCoordinate(0)
                }
                function onTrailsCleared() { trail.path = [] }
                // 최대 점 수를 줄이면 서비스의 궤적과 같은 규칙으로 넘는 점을 앞에서 지워요.
                function onTrailMaxPointsChanged() {
                    while (trail.pathLength() > MapController.trailMaxPoints) trail.removeCoordinate(0)
                }
            }
        }
    }
}
