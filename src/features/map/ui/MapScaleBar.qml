// [0.5.0 5.6.9] specs/features/0.5.0-map.md
/** 지도 가운데 위도의 축척 막대예요.
 * 입력: 지도 map이에요(가운데 위도와 확대를 읽어요).
 * 출력: 위에 "200 m" 꼴 글자, 아래에 그 길이의 밑선과 양 끝 눈금이에요. 막대는 Theme.mapScaleMaxWidth를 넘지 않아요.
 * 실패: 없어요.
 */
import QtQuick
import QtLocation
import Kgc.Ui
import Kgc.Map

Item {
    objectName: "mapScaleBar"
    required property Map map
    implicitWidth: Theme.mapScaleMaxWidth
    implicitHeight: childrenRect.height
    Column {
        Text {
            objectName: "mapScaleLabel"
            text: MapController.scaleFor(map.center.latitude, map.zoomLevel, Theme.mapScaleMaxWidth).label
            font.pixelSize: Theme.captionSize
            color: Theme.osdInk
            style: Text.Outline
            styleColor: Theme.osdShadow
        }
        Item {
            objectName: "mapScaleLine"
            width: MapController.scaleFor(map.center.latitude, map.zoomLevel, Theme.mapScaleMaxWidth).pixels
            height: Theme.mapScaleTickHeight
            // 먼저 Theme.lineWidth만큼 어긋난 그림자를, 그 위에 흰 선을 같은 모양(밑선과 양 끝 눈금)으로 그려요.
            Repeater {
                model: [Theme.osdShadow, Theme.osdInk]
                delegate: Item {
                    required property int index
                    required property color modelData
                    x: index === 0 ? Theme.lineWidth : 0
                    y: index === 0 ? Theme.lineWidth : 0
                    width: parent.width
                    height: parent.height
                    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: Theme.lineWidth * 2; color: modelData }
                    Rectangle { anchors.left: parent.left; width: Theme.lineWidth * 2; height: parent.height; color: modelData }
                    Rectangle { anchors.right: parent.right; width: Theme.lineWidth * 2; height: parent.height; color: modelData }
                }
            }
        }
    }
}
