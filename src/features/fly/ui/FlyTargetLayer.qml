// [0.6.0 5.11.10 T21 T25] specs/features/0.6.0-fly.md
/** 여기로 가기 목표 표식·목표 고도 글·선회 원을 그리는 지도 레이어예요(id flyTarget, order 25 — 홈 위, 기체 화살표 아래).
 * 입력: 얹힌 지도 부품 mapPane과 FlyController의 targetVisible·targetPending·targetCoordinate·targetText·loiterVisible·loiterRadius·loiterText예요.
 * 출력: 목표 표식 flyGoToTarget — 보낸 뒤 확인 전(targetPending)에는 점선 빈 원이고 글이 없어요.
 *       받아들여지면 채운 원과 그 위 "목표 50 m"(flyGoToTargetLabel)예요.
 *       선회 원 flyLoiterCircle(가운데 목표, 반지름은 미터)과 원 동쪽 끝 오른쪽의 "반지름 80 m"(flyLoiterLabel)도 그려요.
 *       모든 지도 부품에 같은 표식이 나와요.
 * 실패: 목표가 없으면 표식이, 선회 원을 그릴 수 없으면(멀티콥터·반지름 모름) 원과 그 글이 보이지 않아요.
 */
import QtQuick
import QtQuick.Shapes
import QtLocation
import QtPositioning
import Kgc.Ui
import Kgc.Fly

MapItemGroup {
    id: layer
    property var mapPane
    MapCircle {
        objectName: "flyLoiterCircle"
        visible: FlyController.loiterVisible
        center: FlyController.targetCoordinate
        radius: FlyController.loiterRadius
        border.width: Theme.flyLoiterWidth
        border.color: Theme.flyLoiter
        color: "transparent"
    }
    // 원의 동쪽 끝 점(방위 90°)에 글의 왼쪽 가운데를 붙여요.
    MapQuickItem {
        objectName: "flyLoiterLabel"
        visible: FlyController.loiterVisible
        coordinate: FlyController.targetCoordinate.atDistanceAndAzimuth(FlyController.loiterRadius, 90)
        anchorPoint.x: 0
        anchorPoint.y: loiterLabel.height / 2
        sourceItem: Text {
            id: loiterLabel
            leftPadding: Theme.mapLabelGap
            text: { const language = translator.language; return FlyController.loiterText }
            font.pixelSize: Theme.captionSize
            color: Theme.osdInk
            style: Text.Outline
            styleColor: Theme.osdShadow
        }
    }
    MapQuickItem {
        objectName: "flyGoToTarget"
        visible: FlyController.targetVisible
        coordinate: FlyController.targetCoordinate
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
                    // 대기(확인 전)는 점선 빈 원, 받아들인 뒤는 실선에 같은 색 채움이에요.
                    strokeStyle: FlyController.targetPending ? ShapePath.DashLine : ShapePath.SolidLine
                    dashPattern: [2, 2]
                    fillColor: FlyController.targetPending ? "transparent" : Theme.flyTarget
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
            Text {
                objectName: "flyGoToTargetLabel"
                visible: !FlyController.targetPending
                text: { const language = translator.language; return FlyController.targetText }
                anchors.bottom: parent.top
                anchors.bottomMargin: Theme.mapLabelGap
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Theme.captionSize
                color: Theme.osdInk
                style: Text.Outline
                styleColor: Theme.osdShadow
            }
        }
    }
}
