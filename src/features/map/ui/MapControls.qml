// [0.5.1 5.9] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.9 T17 T18 T19 T22] specs/features/0.5.0-map.md
/** 지도 오른쪽 아래의 단추 묶음이에요.
 * 입력: 단추가 다룰 지도 부품 mapPane과 열고 닫을 지도 저장 카드 downloadCard예요.
 * 출력: 위에서부터 지도 저장·궤적 지우기·지도 종류(왼쪽에 뜨는 고르기)·따라가기·축소·확대 단추예요.
 *       확대·축소는 지도 가운데를 기준으로 그 방향의 다음 정수 층으로 가요(14.6에서 확대 15, 축소 14).
 * 실패: 확대는 지도의 가장 작은·큰 확대 밖으로 가지 않아요. 따라가기 단추는 활성 기체가 위치를 가질 때만 눌려요.
 * 흐름(따라가기 단추):
 * 1. mapFollow.clicked → mapPane.follow를 뒤집어요(따라가기 ⑤).
 * 2. 켜졌으면 mapPane.map.center = MapController.activeCoordinate로 바로 기체를 가운데에 둬요.
 * 3. 그 뒤 MapController.activePositionChanged마다 MapPane이 가운데를 기체로 옮겨요.
 * 흐름(지도 종류):
 * 1. mapTypeChoice.activated → MapController.mapType — 지도 종류를 설정에 쓰고 모든 지도 부품이 따라 바꿔요.
 */
import QtQuick
import QtQuick.Controls
import Kgc.Ui
import Kgc.Map

Column {
    objectName: "mapControls"
    spacing: Theme.mapControlGap
    // Item으로 적으면 qmllint가 follow·map을 찾지 못해요.
    required property var mapPane
    required property Item downloadCard
    IconButton {
        objectName: "mapStoreButton"
        name: "download"
        hint: qsTr("지도 저장")
        checked: downloadCard.visible
        onClicked: downloadCard.visible = !downloadCard.visible
    }
    IconButton {
        objectName: "mapClearTrails"
        name: "eraser"
        hint: qsTr("궤적 지우기")
        onClicked: MapController.clearTrails()
    }
    IconButton {
        objectName: "mapTypeButton"
        name: "stack-2"
        hint: qsTr("지도 종류")
        checked: popup.visible
        onClicked: popup.visible ? popup.close() : popup.open()
        Popup {
            id: popup
            objectName: "mapTypePopup"
            x: -width - Theme.mapControlGap
            y: (parent.height - height) / 2
            padding: Theme.inset
            // 단추를 다시 누르면 단추가 닫도록, 단추 밖을 누를 때만 스스로 닫아요.
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
            background: Rectangle { color: Theme.surface; border.color: Theme.lineStrong; radius: Theme.radiusSmall }
            PillGroup {
                objectName: "mapTypeChoice"
                model: MapController.mapTypeNames.map(n => qsTranslate("Map", n))
                currentIndex: MapController.mapType
                onActivated: (index) => {
                    MapController.mapType = index
                    popup.close()
                }
            }
        }
    }
    // 누름으로 checked가 바뀌지 않게 checkable 없이 따라가기 상태에 묶어요.
    IconButton {
        objectName: "mapFollow"
        name: "current-location"
        hint: qsTr("기체 따라가기")
        checked: mapPane.follow
        enabled: MapController.activeHasPosition
        onClicked: {
            mapPane.follow = !mapPane.follow
            if (mapPane.follow) mapPane.map.center = MapController.activeCoordinate
        }
    }
    IconButton {
        objectName: "mapZoomOut"
        name: "minus"
        hint: qsTr("축소")
        onClicked: mapPane.map.zoomLevel = MapController.zoomStepTarget(mapPane.map.zoomLevel, -1, mapPane.map.minimumZoomLevel,
                                                                        mapPane.map.maximumZoomLevel)
    }
    IconButton {
        objectName: "mapZoomIn"
        name: "plus"
        hint: qsTr("확대")
        onClicked: mapPane.map.zoomLevel = MapController.zoomStepTarget(mapPane.map.zoomLevel, 1, mapPane.map.minimumZoomLevel,
                                                                        mapPane.map.maximumZoomLevel)
    }
}
