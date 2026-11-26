// [0.5.0 5.6.9] specs/features/0.5.0-map.md
/** 타일 지도와 그 위의 입력·레이어·축척·출처·제어를 묶은 지도 부품이에요. 다른 패널·창에도 같은 것을 놓을 수 있어요.
 * 입력: showControls(제어 단추 보이기), rememberView(보던 곳 저장, 가운데 지도만), follow(따라가기), selecting(영역 고르기)이에요.
 *       타일 받기·저장·지도 종류는 MapController(전역 하나)를 같이 쓰고, 위치·확대는 부품마다 따로예요.
 * 출력: 안의 Map을 map 별칭으로 내놓아요. MapController.lastView()의 가운데·확대에서 시작하고(처음이면 36.5·127.8·7),
 *       MapController.layers의 모든 레이어를 order 순서(z)로 얹어요.
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
        Component.onCompleted: {
            const view = MapController.lastView()
            canvas.zoomLevel = view.zoom
            canvas.center = QtPositioning.coordinate(view.lat, view.lon)
        }
        MapInput {
            map: canvas
            selecting: pane.selecting
            follow: pane.follow
            onUserPanned: pane.follow = false
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
    MapScaleBar {
        map: canvas
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: Theme.inset
    }
    // 출처 글을 오른쪽 아래에 두고, 제어 단추는 그 위 Theme.inset에 둬요.
    Column {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Theme.inset
        spacing: Theme.inset
        MapControls {
            anchors.right: parent.right
            visible: pane.showControls
            mapPane: pane
        }
        Text {
            objectName: "mapAttribution"
            anchors.right: parent.right
            text: qsTr("지도 데이터 © Google")
            font.pixelSize: Theme.captionSize
            color: Theme.osdInk
            style: Text.Outline
            styleColor: Theme.osdShadow
        }
    }
}
