// [0.6.0 5.11.10 T19] specs/features/0.6.0-fly.md
/** 지도를 한 번 누른 곳에 여기로 가기 창과 점선 미리 보기를 여는 지도 레이어예요(id flyGoTo, order 90 — 다른 레이어 위).
 * 입력: 얹힌 지도 부품 mapPane(그 tapped·follow·map)과 FlyController의 canGoTo·readinessChanged·vehicleChanged예요.
 * 출력: 여기로 가기를 할 수 있을 때 누른 좌표 point에 창 flyGoToWindowItem과 미리 보기 flyGoToPreview를 보여요.
 *       창은 좌표에 붙어 지도와 함께 움직이고, 기본은 점의 오른쪽 위예요(자리가 없으면 왼쪽·아래로 뒤집어요).
 *       창을 열면 이 지도 부품의 따라가기를 꺼요(닫아도 다시 켜지 않아요). 지도 부품마다 따로 열려요.
 * 실패: 여기로 가기를 할 수 없으면 누르기에 창이 열리지 않아요. 열린 채 할 수 없게 되거나 활성 기체가 바뀌면 닫혀요.
 * 흐름:
 * 1. MapPane.tapped — 끌지 않은 한 번 누르기의 좌표가 와요.
 * 2. openAt — canGoTo를 보고 point를 정해요. 이미 열려 있으면 창만 옮기고 적어 둔 고도는 그대로예요.
 * 3. FlyGoToWindow.prepare — 화면 점으로 뒤집기를 정하고, 새로 열 때만 고도와 기체를 정해요.
 * 4. 창의 flyGoToConfirm(길게 누르기) → FlyController.goTo → FlyService::goTo, 보냈으면 sent로 창이 닫혀요.
 */
import QtQuick
import QtQuick.Shapes
import QtLocation
import QtPositioning
import Kgc.Ui
import Kgc.Fly

MapItemGroup {
    // 뿌리 id를 layer로 두면 Item의 layer 속성을 가려 qmllint가 경고하므로(id-shadows-member) 레이어 id와 같은 이름을 써요.
    id: flyGoTo
    property var mapPane
    // 연 창의 좌표예요. 무효 좌표면 창이 닫힌 거예요.
    property var point: QtPositioning.coordinate()
    readonly property bool open: point.isValid
    function openAt(coordinate) {
        if (!FlyController.canGoTo) return
        const fresh = !flyGoTo.open
        point = coordinate
        flyGoTo.mapPane.follow = false
        goToWindow.prepare(flyGoTo.mapPane.map.fromCoordinate(coordinate, false), flyGoTo.mapPane.map.width, fresh)
    }
    function close() { point = QtPositioning.coordinate() }
    Connections {
        target: flyGoTo.mapPane
        function onTapped(coordinate, position) { flyGoTo.openAt(coordinate) }
    }
    Connections {
        target: FlyController
        function onReadinessChanged() { if (!FlyController.canGoTo) flyGoTo.close() }
        function onVehicleChanged() { flyGoTo.close() }
    }
    // 목표 표식과 같은 크기·색의 점선 빈 원이에요(이 지도 부품에만 있어요).
    MapQuickItem {
        objectName: "flyGoToPreview"
        visible: flyGoTo.open
        coordinate: flyGoTo.point
        anchorPoint.x: Theme.flyTargetSize / 2
        anchorPoint.y: Theme.flyTargetSize / 2
        sourceItem: Item {
            width: Theme.flyTargetSize
            height: Theme.flyTargetSize
            Shape {
                anchors.fill: parent
                ShapePath {
                    strokeColor: Theme.flyTarget
                    strokeWidth: Theme.flyTargetLineWidth
                    strokeStyle: ShapePath.DashLine
                    dashPattern: [2, 2]
                    fillColor: "transparent"
                    PathAngleArc {
                        centerX: Theme.flyTargetSize / 2
                        centerY: Theme.flyTargetSize / 2
                        radiusX: Theme.flyTargetSize / 2 - Theme.flyTargetLineWidth
                        radiusY: Theme.flyTargetSize / 2 - Theme.flyTargetLineWidth
                        startAngle: 0
                        sweepAngle: 360
                    }
                }
            }
        }
    }
    // 뒤집지 않으면 창의 왼쪽 아래 모서리가 점에서 오른쪽·위로 inset만큼, 뒤집으면 오른쪽 위 모서리가 왼쪽·아래로 inset만큼 떨어져요.
    MapQuickItem {
        objectName: "flyGoToWindowItem"
        visible: flyGoTo.open
        coordinate: flyGoTo.point
        anchorPoint.x: goToWindow.flipX ? goToWindow.width + Theme.inset : -Theme.inset
        anchorPoint.y: goToWindow.flipY ? -Theme.inset : goToWindow.height + Theme.inset
        sourceItem: FlyGoToWindow {
            id: goToWindow
            point: flyGoTo.point
            onCancelled: flyGoTo.close()
            onSent: flyGoTo.close()
        }
    }
}
