// [0.5.0 5.6.9] specs/features/0.5.0-map.md
/** 지도 위의 끌기·휠·핀치를 지도 이동·확대와 영역 고르기로 바꿔요.
 * 입력: 지도 map, 영역 고르기 중인지 selecting, 따라가기 중인지 follow예요.
 * 출력: 지도를 옮기고 확대하며, 사용자가 지도를 끌면 userPanned, 고르는 동안 selectionMoved, 손을 떼면 selectionDone(geoRectangle)을 내요.
 *       따라가기 중의 휠·핀치는 지도 가운데(기체)를 기준으로 확대해요. 방위·기울기는 바꾸지 않아요.
 * 실패: 끌지 않은 클릭은 아무 일도 하지 않아요. 다른 항목·핸들러가 이미 쥔 누름은 가져가지 않아요.
 */
import QtQuick
import QtLocation
import QtPositioning

Item {
    id: input
    objectName: "mapInput"
    anchors.fill: parent
    required property Map map
    property bool selecting: false
    property bool follow: false
    property var lastArea
    property var startCentroid
    signal userPanned()
    signal selectionMoved(var area)
    signal selectionDone(var area)
    // 확대를 범위 안으로 자르고, 따라가기가 아니면 그 점 아래 좌표가 제자리에 있게 해요.
    function zoomAround(level, coordinate, point) {
        input.map.zoomLevel = Math.max(input.map.minimumZoomLevel, Math.min(input.map.maximumZoomLevel, level))
        if (!input.follow) input.map.alignCoordinateToPoint(coordinate, point)
    }
    // 화면 두 점을 x·y마다 정렬해 왼쪽 위·오른쪽 아래 좌표로 만들어요. 경도로 정렬하지 않아 날짜 변경선을 넘는 영역도 맞아요.
    function areaBetween(a, b) {
        const topLeft = input.map.toCoordinate(Qt.point(Math.min(a.x, b.x), Math.min(a.y, b.y)), false)
        const bottomRight = input.map.toCoordinate(Qt.point(Math.max(a.x, b.x), Math.max(a.y, b.y)), false)
        return QtPositioning.rectangle(topLeft, bottomRight)
    }
    DragHandler {
        objectName: "mapDrag"
        target: null
        enabled: !input.selecting
        acceptedButtons: Qt.LeftButton
        // CanTakeOver 깃발을 두지 않아 패널·카드·단추가 쥔 누름을 지도 끌기가 가져가지 않아요.
        grabPermissions: PointerHandler.ApprovesTakeOverByAnything
        onActiveChanged: if (active) input.userPanned()
        onTranslationChanged: (delta) => input.map.pan(-delta.x, -delta.y)
    }
    DragHandler {
        objectName: "mapSelectDrag"
        target: null
        enabled: input.selecting
        acceptedButtons: Qt.LeftButton
        grabPermissions: PointerHandler.ApprovesTakeOverByAnything
        onTranslationChanged: {
            if (!active) return
            input.lastArea = input.areaBetween(centroid.pressPosition, centroid.position)
            input.selectionMoved(input.lastArea)
        }
        // 문턱을 넘긴 움직임은 translation을 바꾸지 않아요. 그래서 활성이 되는 순간에도 영역을 정해야
        // 빠르게 끌어도 영역이 생기고, 지난 고르기의 영역이 selectionDone으로 나가지 않아요.
        onActiveChanged: {
            if (active) {
                input.lastArea = input.areaBetween(centroid.pressPosition, centroid.position)
                input.selectionMoved(input.lastArea)
            } else {
                input.selectionDone(input.lastArea)
            }
        }
    }
    WheelHandler {
        id: wheelHandler
        objectName: "mapWheel"
        target: null
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        // 세로 각도만 봐요. 수정키와 pixelDelta는 보지 않아 Shift·Ctrl+휠도 같은 확대예요.
        onWheel: (event) => input.zoomAround(input.map.zoomLevel + event.angleDelta.y / 120,
                                             input.map.toCoordinate(wheelHandler.point.position, false), wheelHandler.point.position)
    }
    PinchHandler {
        objectName: "mapPinch"
        target: null
        rotationAxis.enabled: false
        grabPermissions: PointerHandler.TakeOverForbidden
        onActiveChanged: if (active) input.startCentroid = input.map.toCoordinate(centroid.position, false)
        onScaleChanged: (delta) => input.zoomAround(input.map.zoomLevel + Math.log2(delta), input.startCentroid, centroid.position)
    }
}
