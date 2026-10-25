// [0.0.2 T8 T14 T18] specs/features/0.0.2-theme.md
// [0.0.2 T24] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls
import QtQml.Models
import Kgc.Ui
Item {
    id: center
    objectName: "centerArea"
    property alias mapStage: stage
    property real viewportLeft: 0
    property real viewportRight: 0
    property real viewportTop: 0
    property real viewportBottom: 0
    property var selectedPanel: {
        const panelId = shellController.activePanelId
        for (let i = 0; i < panels.count; ++i) {
            const entry = panels.objectAt(i)
            if (entry && entry.panelId === panelId) return entry
        }
        return null
    }
    property string panelTitle: {
        // 동적 메뉴 제목도 언어 변경 시 다시 번역되도록 이 속성을 바인딩 의존성으로 읽어요.
        const language = translator.language
        return selectedPanel ? qsTranslate("Menu", selectedPanel.titleKey) : ""
    }
    Instantiator {
        id: panels
        model: panelRegistry
        delegate: QtObject {
            required property var model
            property string panelId: model.id
            property string titleKey: model.titleKey
            property url component: model.component
        }
    }
    Item {
        id: stage
        objectName: "mapStage"
        anchors.fill: parent
        Rectangle {
            objectName: "backgroundLayer"
            anchors.fill: parent
            color: Theme.window
            Canvas {
                objectName: "mapPlaceholder"
                anchors.fill: parent
                visible: uiSettingValues.backgroundKind === "checker"
                property color patternA: Theme.mapPatternA
                property color patternB: Theme.mapPatternB
                property real cellSize: Theme.inset * 3
                onPatternAChanged: requestPaint()
                onPatternBChanged: requestPaint()
                onCellSizeChanged: requestPaint()
                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()
                onPaint: {
                    const ctx = getContext("2d")
                    for (let y = 0; y < height; y += cellSize) {
                        for (let x = 0; x < width; x += cellSize) {
                            ctx.fillStyle = ((x / cellSize + y / cellSize) % 2) ? patternA : patternB
                            ctx.fillRect(x, y, cellSize, cellSize)
                        }
                    }
                }
            }
            Image {
                id: backgroundImage
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                source: uiSettingValues.backgroundKind === "image" ? uiSettingValues.backgroundImageSource : ""
                visible: status === Image.Ready
                // 빈 경로에는 읽기 오류 시그널이 없으므로 별도로 같은 경고 통로를 써요.
                function reportMissing() {
                    if (uiSettingValues.backgroundKind === "image"
                            && (status === Image.Error || uiSettingValues.backgroundImage === ""))
                        uiSettingValues.reportBackgroundImageMissing(uiSettingValues.backgroundImage)
                }
                onStatusChanged: reportMissing()
                Component.onCompleted: reportMissing()
                Connections {
                    target: uiSettingValues
                    function onBackgroundKindChanged() { backgroundImage.reportMissing() }
                }
            }
        }
        Repeater { model: mapLayerSlot; delegate: Item {} }
    }
    Item {
        objectName: "centerViewport"
        anchors.fill: parent
        anchors.leftMargin: center.viewportLeft
        anchors.rightMargin: center.viewportRight
        anchors.topMargin: center.viewportTop
        anchors.bottomMargin: center.viewportBottom
        Text { anchors.centerIn: parent; text: qsTr("지도"); color: Theme.inkMuted; font.pixelSize: Theme.titleSize }
        GlassSurface {
            id: overlay
            source: stage
            objectName: "panelOverlay"
            anchors.fill: parent
            anchors.margins: Theme.panelInset
            visible: center.selectedPanel !== null
            Item {
                id: header
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: Theme.panelHeaderHeight
                Text {
                    objectName: "panelTitle"
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.panelInset
                    anchors.verticalCenter: parent.verticalCenter
                    text: center.panelTitle
                    color: Theme.ink
                    font.pixelSize: Theme.titleSize
                    font.bold: true
                }
                IconButton {
                    objectName: "panelClose"
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: Theme.panelCloseSize
                    height: Theme.panelCloseSize
                    name: "x"
                    hint: qsTr("닫기")
                    onClicked: shellController.togglePanel(shellController.activePanelId)
                }
            }
            Text {
                objectName: "placeholderBody"
                anchors.centerIn: parent
                visible: overlay.visible && loader.source.toString() === ""
                text: center.panelTitle + "\n" + qsTr("준비 중")
                horizontalAlignment: Text.AlignHCenter
                color: Theme.inkMuted
                font.pixelSize: Theme.fontSize
            }
            Loader {
                id: loader
                anchors.top: header.bottom
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Theme.inset
                source: center.selectedPanel ? center.selectedPanel.component : ""
                onLoaded: {
                    if ("glassSource" in item) item.glassSource = stage
                    item.onArg(shellController.panelArg)
                }
            }
            Connections {
                target: shellController
                function onPanelArgRequested(id, arg) {
                    if (id === shellController.activePanelId && loader.item) loader.item.onArg(arg)
                }
            }
        }
    }
}
