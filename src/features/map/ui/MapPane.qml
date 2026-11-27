// [0.5.0 5.6.9 T17 T18 T21 T22 T24] specs/features/0.5.0-map.md
/** 타일 지도와 그 위의 입력·레이어·축척·출처·제어·지도 저장 카드를 묶은 지도 부품이에요. 다른 패널·창에도 같은 것을 놓을 수 있어요.
 * 입력: showControls(제어 단추와 카드), rememberView(보던 곳 저장, 가운데 지도만), follow(따라가기), selecting(영역 고르기)이에요.
 *       타일 받기·저장·지도 종류는 MapController(전역 하나)를 같이 쓰고, 위치·확대·따라가기는 부품마다 따로예요.
 * 출력: 안의 Map을 map 별칭으로 내놓아요. MapController.lastView()의 가운데·확대에서 시작하고(처음이면 36.5·127.8·7),
 *       MapController.layers의 모든 레이어를 order 순서(z)로 얹어요. 따라가기가 켜져 있으면 활성 기체를 가운데에 둬요.
 * 실패: 레이어 컴포넌트를 만들지 못하면 MapController.reportLayerFailed로 남기고 그 레이어만 빼요.
 */
import QtQuick
import QtQml
import QtQml.Models
import QtLocation
import QtPositioning
import Kgc.Ui
import Kgc.Map

Item {
    id: pane
    property bool showControls: true
    property bool rememberView: false
    property bool follow: false
    property bool selecting: false
    readonly property alias map: canvas
    // 활성 기체가 바뀌었는데 아직 위치가 없어, 첫 위치가 오면 따라가기를 켤 차례인지예요(부품 안에서만 써요).
    property bool armPending: false
    // 영역 고르기에 들어가면 따라가기를 끄고, 위치를 기다리던 자동 켜기도 거둬요(따라가기 ④).
    onSelectingChanged: {
        if (!selecting) return
        follow = false
        armPending = false
    }
    // 지도 데이터를 지우거나 미리 받는 Map 함수는 부르지 않아요(작업 폴더의 파일을 지우거나 이웃 확대 층까지 받아요).
    Map {
        id: canvas
        objectName: "mapCanvas"
        anchors.fill: parent
        plugin: Plugin { name: "kgc" }
        color: Theme.window
        copyrightsVisible: false
        minimumZoomLevel: MapController.minZoom
        maximumZoomLevel: MapController.maxZoom
        // 보던 곳에서 시작한 뒤, 지금 활성 기체로 따라가기를 한 번 정해요(따라가기 ②, 시작 순서를 지키려고 같은 곳에서 해요).
        Component.onCompleted: {
            const view = MapController.lastView()
            canvas.zoomLevel = view.zoom
            canvas.center = QtPositioning.coordinate(view.lat, view.lon)
            const arm = MapController.activeSysid !== -1 && MapController.autoFollow
            pane.follow = arm && MapController.activeHasPosition
            pane.armPending = arm && !MapController.activeHasPosition
            if (pane.follow) canvas.center = MapController.activeCoordinate
        }
        MapInput {
            map: canvas
            selecting: pane.selecting
            follow: pane.follow
            onUserPanned: pane.follow = false
            onSelectionMoved: (area) => card.area = area
            onSelectionDone: (chosen) => card.choose(chosen)
        }
        // 고르는 중(끈 뒤)과 계획 단계에서 고른 영역을 보여 줘요. 다른 레이어보다 위예요.
        MapRectangle {
            objectName: "mapSelectionRect"
            border.width: Theme.mapSelectionWidth
            border.color: Theme.mapSelection
            color: Qt.alpha(Theme.mapSelection, 0.15)
            z: 1000
            visible: card.visible && card.area !== undefined && (card.step === "select" || card.step === "plan")
            topLeft: card.area !== undefined ? card.area.topLeft : QtPositioning.coordinate()
            bottomRight: card.area !== undefined ? card.area.bottomRight : QtPositioning.coordinate()
        }
    }
    // 지도 종류 셋이 준비된 뒤에 전역 지도 종류(0·1·2)를 엔진 지도 종류(id 1·2·3)로 맞춰요.
    // 셋이 들어오는 순간 when이 value보다 먼저 참이 되어도 빈 값을 쓰지 않도록, 그 전에는 지금 종류를 값으로 둬요.
    Binding {
        target: canvas
        property: "activeMapType"
        value: canvas.supportedMapTypes.length === 3 ? canvas.supportedMapTypes[MapController.mapType] : canvas.activeMapType
        when: canvas.supportedMapTypes.length === 3
    }
    // MapItemGroup은 나중에 넣은 자식 항목을 지도에 얹지 않으므로, 레이어마다 만들어 addMapItemGroup으로 얹어요.
    Instantiator {
        model: MapController.layers
        delegate: QtObject {
            required property var model
            property Item layer: null
            Component.onCompleted: {
                const component = Qt.createComponent(model.component, Component.PreferSynchronous)
                if (component.status !== Component.Ready) {
                    MapController.reportLayerFailed(model.id, component.errorString())
                    return
                }
                // z가 없으면 나중에 등록한 레이어가 위로 와요.
                layer = component.createObject(canvas, { mapPane: pane, objectName: "mapLayer_" + model.id, z: model.order })
                canvas.addMapItemGroup(layer)
            }
            Component.onDestruction: {
                if (!layer) return
                canvas.removeMapItemGroup(layer)
                layer.destroy()
            }
        }
    }
    // 따라가기 ①②③④: 부품마다 활성 기체의 위치·바뀜과 자동 켜기 설정을 들어요.
    Connections {
        target: MapController
        function onActivePositionChanged() {
            if (!MapController.activeHasPosition) return
            if (pane.armPending) {
                pane.follow = true
                pane.armPending = false
            }
            if (pane.follow) canvas.center = MapController.activeCoordinate
        }
        // 활성 기체가 바뀌면 사용자가 꺼 두었어도 자동 켜기 설정대로 다시 정해요. 같은 기체 동안은 다시 켜지 않아요.
        function onActiveSysidChanged() {
            const arm = MapController.activeSysid !== -1 && MapController.autoFollow
            pane.follow = arm && MapController.activeHasPosition
            pane.armPending = arm && !MapController.activeHasPosition
            if (pane.follow) canvas.center = MapController.activeCoordinate
        }
        // 설정을 끄면 기다리던 자동 켜기만 거두고 지금의 따라가기는 그대로 둬요.
        function onAutoFollowChanged() { if (!MapController.autoFollow) pane.armPending = false }
    }
    // 보던 곳 저장은 rememberView인 부품만, 지도가 saveViewDelayMs 동안 멈춘 뒤에 해요.
    TimerBox {
        id: saveViewTimer
        interval: MapController.saveViewDelayMs
        onTriggered: MapController.saveView(canvas.center.latitude, canvas.center.longitude, canvas.zoomLevel)
    }
    Connections {
        target: canvas
        enabled: pane.rememberView
        function onCenterChanged() { saveViewTimer.restart() }
        function onZoomLevelChanged() { saveViewTimer.restart() }
    }
    MapScaleBar {
        map: canvas
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: Theme.inset
    }
    // 출처 글·제어·카드는 모두 pane의 직접 자식이에요(앵커는 형제끼리만 돼요).
    Text {
        id: attribution
        objectName: "mapAttribution"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Theme.inset
        text: qsTr("지도 데이터 © Google")
        font.pixelSize: Theme.captionSize
        color: Theme.osdInk
        style: Text.Outline
        styleColor: Theme.osdShadow
    }
    MapControls {
        id: controls
        visible: pane.showControls
        mapPane: pane
        downloadCard: card
        anchors.right: parent.right
        anchors.bottom: attribution.top
        anchors.rightMargin: Theme.inset
        anchors.bottomMargin: Theme.inset
    }
    MapDownloadCard {
        id: card
        mapPane: pane
        visible: false
        anchors.right: controls.left
        anchors.rightMargin: Theme.mapControlGap
        anchors.bottom: controls.bottom
    }
}
