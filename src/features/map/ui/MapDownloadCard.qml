// [0.5.1 5.9] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.9 T22 T23 T25] specs/features/0.5.0-map.md
/** 지도 제어 왼쪽에 뜨는 "지도 저장" 카드예요. 저장 크기 보기·비우기와 영역 받기(고르기 → 계획 → 받기)를 해요.
 * 입력: 카드가 다룰 지도 부품 mapPane이에요. 지도 부품이 고른 영역을 choose(chosen)로 넘겨요.
 * 출력: step이 "view"·"select"·"plan"·"download" 중 하나이고, 단계마다 Column 하나를 보여요.
 *       보이게 될 때 받는 중이면 "download", 아니면 "view"로 열고 저장소 크기를 다시 물어요.
 * 실패: 계획 오류(확대 범위·100,000개)와 시작 오류(저장소 고장·받는 중)는 mapPlanLimit에 보이고 계획 단계에 머물러요.
 * 흐름(영역 받기):
 * 1. mapDownloadArea → step "select", mapPane.selecting = true — 지도 끌기가 영역 고르기로 바뀌어요.
 * 2. 지도 부품의 selectionDone → choose — 영역과 기본 확대 범위(지금 확대의 내림 ~ +4)를 정하고 step "plan"이에요.
 * 3. mapDownloadStart → MapController.startDownload — 참이면 step "download"에서 JobView로 진행을 보여요.
 * 4. 끝나면 결과 글과 mapDownloadClose → MapController.dismissDownload, refreshStoreSize, step "view"예요.
 */
import QtQuick
import Kgc.Ui
import Kgc.Map

Item {
    id: card
    objectName: "mapDownloadCard"
    width: Theme.mapCardWidth
    required property var mapPane
    property string step: "view"
    property var area
    property int zoomFrom
    property int zoomTo
    // 고른 영역과 기본 확대 범위로 계획 단계를 열어요. NumberBox는 고칠 때 value 바인딩을 끊으므로 여기서 값을 넣어요.
    function choose(chosen) {
        if (!chosen) return
        area = chosen
        zoomFrom = Math.min(MapController.maxZoom, Math.max(MapController.minZoom, Math.floor(mapPane.map.zoomLevel)))
        zoomTo = Math.min(zoomFrom + 4, MapController.maxZoom)
        zoomFromBox.value = zoomFrom
        zoomToBox.value = zoomTo
        mapPane.selecting = false
        step = "plan"
    }
    onVisibleChanged: {
        if (visible) {
            step = MapController.downloadJob !== null ? "download" : "view"
            MapController.refreshStoreSize()
        } else if (step === "select" || step === "plan") {
            // 닫으면 고르기·계획은 버려요. 받는 중인 작업은 계속돼요.
            mapPane.selecting = false
            step = "view"
        }
    }
    // 카드 위에서 시작한 누르기·끌기·휠이 아래 지도로 내려가지 않게 받아요.
    MouseArea {
        objectName: "mapCardInputBlock"
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        onWheel: (wheel) => wheel.accepted = true
    }
    // 내용의 TapHandler가 먼저 받아 MouseArea까지 오지 않은 누름도 독점으로 잡아 지도 끌기가 가져가지 못하게 해요.
    TapHandler { objectName: "mapCardTapBlock"; gesturePolicy: TapHandler.DragWithinBounds }
    Card {
        anchors.left: parent.left
        anchors.right: parent.right
        source: card.mapPane.map
        title: card.step === "view" ? qsTr("지도 저장") : qsTr("영역 받기")
        // 카드 높이는 안의 Card 높이를 따라가요.
        Component.onCompleted: card.height = Qt.binding(() => implicitHeight)
        Column {
            width: parent.width
            spacing: Theme.inset
            visible: card.step === "view"
            Text {
                objectName: "mapStoreText"
                width: parent.width
                text: qsTr("저장한 지도 %1 · 미리 받은 것 %2").arg(MapController.storeTotalText).arg(MapController.storePinnedText)
                color: Theme.inkMuted
                font.pixelSize: Theme.fontSize
                wrapMode: Text.Wrap
            }
            Text {
                objectName: "mapStoreError"
                width: parent.width
                visible: text !== ""
                text: MapController.storeError
                color: Theme.danger
                font.pixelSize: Theme.fontSize
                wrapMode: Text.Wrap
            }
            Flow {
                width: parent.width
                spacing: Theme.inset
                // 새로 고를 때는 지난 영역을 지워, 끌기 전에는 고르기 칸이 보이지 않게 해요.
                ActionButton {
                    objectName: "mapDownloadArea"
                    text: qsTr("영역 받기")
                    onClicked: {
                        card.area = undefined
                        card.step = "select"
                        card.mapPane.selecting = true
                    }
                }
                HoldToConfirm { objectName: "mapStoreClear"; text: qsTr("비우기"); onConfirmed: MapController.clearStore() }
            }
        }
        Column {
            width: parent.width
            spacing: Theme.inset
            visible: card.step === "select"
            Text {
                objectName: "mapSelectHint"
                width: parent.width
                text: qsTr("지도에서 끌어서 영역을 고르세요")
                color: Theme.inkMuted
                font.pixelSize: Theme.fontSize
                wrapMode: Text.Wrap
            }
            ActionButton {
                objectName: "mapSelectCancel"
                text: qsTr("취소")
                kind: "secondary"
                onClicked: {
                    card.step = "view"
                    card.mapPane.selecting = false
                }
            }
        }
        Column {
            width: parent.width
            spacing: Theme.inset
            visible: card.step === "plan"
            Text { text: qsTr("확대 범위"); color: Theme.ink; font.pixelSize: Theme.fontSize }
            Row {
                width: parent.width
                spacing: Theme.inset
                // 기본 폭(Theme.inputWidth) 둘은 카드에 들어가지 않아 물결표를 뺀 나머지를 반씩 나눠요.
                NumberBox {
                    id: zoomFromBox
                    objectName: "mapZoomFrom"
                    from: MapController.minZoom
                    to: MapController.maxZoom
                    width: (parent.width - tilde.width - parent.spacing * 2) / 2
                    onValueModified: (value) => card.zoomFrom = value
                }
                Text {
                    id: tilde
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("~")
                    color: Theme.ink
                    font.pixelSize: Theme.fontSize
                }
                NumberBox {
                    id: zoomToBox
                    objectName: "mapZoomTo"
                    from: MapController.minZoom
                    to: MapController.maxZoom
                    width: (parent.width - tilde.width - parent.spacing * 2) / 2
                    onValueModified: (value) => card.zoomTo = value
                }
            }
            // 영역이 없을 때(다른 단계)는 MapController를 부르지 않아요.
            Text {
                objectName: "mapPlanCount"
                width: parent.width
                text: {
                    if (card.area === undefined) return ""
                    // estimateText는 C++에서 지도 종류와 타일 화소 배수를 읽어 알림이 없으므로, 둘이 바뀌면 다시 구하도록 여기서 읽어요.
                    void MapController.mapType
                    void MapController.tilePixelRatio
                    const tiles = MapController.tileCount(card.area, card.zoomFrom, card.zoomTo)
                    return qsTr("타일 %1개 · 약 %2").arg(tiles.toLocaleString(Qt.locale("en_US"), 'f', 0)).arg(MapController.estimateText(tiles))
                }
                color: Theme.inkMuted
                font.pixelSize: Theme.fontSize
                wrapMode: Text.Wrap
            }
            Text {
                objectName: "mapPlanType"
                width: parent.width
                text: qsTr("지금 지도 종류(%1)로 받아요").arg(qsTranslate("Map", MapController.mapTypeNames[MapController.mapType]))
                color: Theme.inkMuted
                font.pixelSize: Theme.fontSize
                wrapMode: Text.Wrap
            }
            Text {
                objectName: "mapPlanLimit"
                width: parent.width
                visible: text !== ""
                text: card.area === undefined ? "" : MapController.planError(card.area, card.zoomFrom, card.zoomTo) || MapController.downloadError
                color: Theme.danger
                font.pixelSize: Theme.fontSize
                wrapMode: Text.Wrap
            }
            Flow {
                width: parent.width
                spacing: Theme.inset
                ActionButton {
                    objectName: "mapDownloadStart"
                    text: qsTr("받기")
                    enabled: card.area !== undefined && MapController.planError(card.area, card.zoomFrom, card.zoomTo) === ""
                    onClicked: if (MapController.startDownload(card.area, card.zoomFrom, card.zoomTo)) card.step = "download"
                }
                ActionButton {
                    objectName: "mapReselect"
                    text: qsTr("다시 고르기")
                    kind: "secondary"
                    onClicked: {
                        card.area = undefined
                        card.step = "select"
                        card.mapPane.selecting = true
                    }
                }
                ActionButton { objectName: "mapPlanCancel"; text: qsTr("취소"); kind: "secondary"; onClicked: card.step = "view" }
            }
        }
        Column {
            width: parent.width
            spacing: Theme.inset
            visible: card.step === "download"
            JobView {
                objectName: "mapDownloadJob"
                width: parent.width
                job: MapController.downloadJob
                showError: false
                visible: MapController.downloadRunning
            }
            Text {
                objectName: "mapDownloadResult"
                width: parent.width
                visible: !MapController.downloadRunning
                text: MapController.downloadResultText
                color: MapController.downloadFailed ? Theme.danger : Theme.ink
                font.pixelSize: Theme.fontSize
                wrapMode: Text.Wrap
            }
            ActionButton {
                objectName: "mapDownloadClose"
                visible: !MapController.downloadRunning
                text: qsTr("닫기")
                kind: "secondary"
                onClicked: {
                    MapController.dismissDownload()
                    MapController.refreshStoreSize()
                    card.step = "view"
                }
            }
        }
    }
}
