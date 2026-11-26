// [0.5.0 5.6.9] specs/features/0.5.0-map.md
/** 지도 오른쪽 아래의 단추 묶음이에요.
 * 입력: 단추가 다룰 지도 부품 mapPane이에요.
 * 출력: 위에서부터 지도 종류(왼쪽에 뜨는 고르기)·축소·확대 단추예요. 확대·축소는 지도 가운데를 기준으로 한 단계씩이에요.
 * 실패: 확대는 지도의 가장 작은·큰 확대 밖으로 가지 않아요.
 * 흐름:
 * 1. mapTypeChoice.activated → MapController.mapType — 지도 종류를 설정에 쓰고 모든 지도 부품이 따라 바꿔요.
 */
import QtQuick
import QtQuick.Controls
import Kgc.Ui
import Kgc.Map

Column {
    objectName: "mapControls"
    spacing: Theme.mapControlGap
    // Item으로 적으면 qmllint가 map을 찾지 못해요.
    required property var mapPane
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
    IconButton {
        objectName: "mapZoomOut"
        name: "minus"
        hint: qsTr("축소")
        onClicked: mapPane.map.zoomLevel = Math.max(mapPane.map.minimumZoomLevel, mapPane.map.zoomLevel - 1)
    }
    IconButton {
        objectName: "mapZoomIn"
        name: "plus"
        hint: qsTr("확대")
        onClicked: mapPane.map.zoomLevel = Math.min(mapPane.map.maximumZoomLevel, mapPane.map.zoomLevel + 1)
    }
}
