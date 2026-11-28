// [0.6.0 5.9] specs/features/0.6.0-fly.md
// [0.5.1 5.8] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.9] specs/features/0.5.0-map.md
/** 지도 위의 누르기·끌기·휠·핀치를 지도 이동·확대와 영역 고르기로 바꿔요.
 * 입력: 지도 map, 영역 고르기 중인지 selecting, 따라가기 중인지 follow예요.
 * 출력: 지도를 옮기고 확대하며, 사용자가 지도를 끌면 userPanned, 고르는 동안 selectionMoved, 손을 떼면 selectionDone(geoRectangle)을 내요.
 *       끌지 않은 한 번 누르기는 지도를 바꾸지 않고 tapped(좌표, 화면 점)를 내요.
 *       확대는 정수 층으로 맞춰요(최소 확대만 예외). 휠은 모은 값 120마다 한 층, 핀치는 놓을 때 가까운 층으로 바로 맞춰요.
 *       따라가기 중의 휠·핀치는 지도 가운데(기체)를 기준으로 확대해요. 방위·기울기는 바꾸지 않아요.
 * 실패: 영역 고르는 중의 누르기는 tapped를 내지 않아요. 다른 항목·핸들러가 이미 쥔 누름은 가져가지 않아요.
 */
import QtQuick
import QtLocation
import QtPositioning
import Kgc.Map

Item {
    id: input
    objectName: "mapInput"
    anchors.fill: parent
    required property Map map
    property bool selecting: false
    property bool follow: false
    property var lastArea
    property var startCentroid
    property var dragAnchor
    // 이 몸짓에서 모은 휠 값과, 이 몸짓이 잔 값 흐름(정밀 터치패드·부드러운 휠)인지예요.
    property real wheelSum: 0
    property bool wheelFine: false
    // 마지막으로 층을 넘긴 때(Date.now())예요. 몸짓이 끝나면 0으로 돌려요.
    property double lastStepTime: 0
    // 핀치를 시작할 때의 확대와 핀치의 마지막 가운데 점이에요(놓은 뒤에는 centroid를 믿을 수 없어요).
    property real pinchStartZoom: 0
    property point pinchPoint
    signal userPanned()
    signal selectionMoved(var area)
    signal selectionDone(var area)
    signal tapped(var coordinate, point position)
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
        // 누른 곳의 좌표를 손 아래에 계속 맞춰요. Map.pan(int, int)은 움직임의 소수 부분을 버려
        // 화면 배율 125·150 %나 터치에서 천천히 끌면 지도가 손을 따라오지 못해요.
        onActiveChanged: {
            if (!active) return
            input.dragAnchor = input.map.toCoordinate(centroid.pressPosition, false)
            input.userPanned()
            input.map.alignCoordinateToPoint(input.dragAnchor, centroid.position)
        }
        onTranslationChanged: if (active) input.map.alignCoordinateToPoint(input.dragAnchor, centroid.position)
    }
    // 끌기 문턱을 넘으면 mapDrag가 누름을 가져가 누르기가 되지 않아요.
    TapHandler {
        objectName: "mapTap"
        enabled: !input.selecting
        acceptedButtons: Qt.LeftButton
        gesturePolicy: TapHandler.DragThreshold
        grabPermissions: PointerHandler.ApprovesTakeOverByAnything
        onTapped: (eventPoint) => input.tapped(input.map.toCoordinate(eventPoint.position, false), eventPoint.position)
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
        // 휠이 이만큼 쉬면 비활성이 되어 한 몸짓이 끝나요.
        activeTimeout: MapController.wheelGestureTimeout
        onActiveChanged: if (!active) { input.wheelSum = 0; input.wheelFine = false; input.lastStepTime = 0 }
        // 세로 각도만 봐요. 수정키와 pixelDelta는 보지 않아 Shift·Ctrl+휠(Windows 터치패드 핀치)도 같은 확대예요.
        // 손을 뗀 뒤의 관성 사건은 버려요. 층 판정은 MapController.wheelStep, 갈 층은 zoomStepTarget이 정해요.
        onWheel: (event) => {
            if (event.phase === Qt.ScrollMomentum) return
            const now = Date.now()
            const wheel = MapController.wheelStep(input.wheelSum, input.wheelFine, event.angleDelta.y, now - input.lastStepTime)
            input.wheelSum = wheel.sum
            input.wheelFine = wheel.fine
            if (wheel.steps === 0) return
            input.lastStepTime = now
            input.zoomAround(MapController.zoomStepTarget(input.map.zoomLevel, wheel.steps, input.map.minimumZoomLevel, input.map.maximumZoomLevel),
                             input.map.toCoordinate(wheelHandler.point.position, false), wheelHandler.point.position)
        }
    }
    PinchHandler {
        objectName: "mapPinch"
        target: null
        rotationAxis.enabled: false
        grabPermissions: PointerHandler.TakeOverForbidden
        // 하는 동안은 부드럽게 바뀌고, 놓으면 가까운 정수 층으로 애니메이션 없이 바로 맞춰요.
        onActiveChanged: {
            if (active) {
                input.startCentroid = input.map.toCoordinate(centroid.position, false)
                input.pinchStartZoom = input.map.zoomLevel
                input.pinchPoint = centroid.position
                return
            }
            input.zoomAround(MapController.pinchSnapTarget(input.pinchStartZoom, input.map.zoomLevel, input.map.minimumZoomLevel,
                                                           input.map.maximumZoomLevel), input.startCentroid, input.pinchPoint)
        }
        onScaleChanged: (delta) => {
            input.pinchPoint = centroid.position
            input.zoomAround(input.map.zoomLevel + Math.log2(delta), input.startCentroid, centroid.position)
        }
    }
}
