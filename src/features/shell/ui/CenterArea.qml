// [0.0.1 T4–T9] specs/features/0.0.1-shell.md
import QtQuick
import QtQuick.Controls
import QtQml.Models
import Kgc.Ui
Rectangle {
    id: center
    objectName: "centerArea"
    color: Theme.window
    border.color: Theme.line
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
    Text {
        objectName: "mapPlaceholder"
        anchors.centerIn: parent
        text: qsTr("지도")
        color: Theme.inkMuted
        font.pixelSize: Theme.titleSize
    }
    Repeater { model: mapLayerSlot; delegate: Item {} }
    Rectangle {
        id: overlay
        objectName: "panelOverlay"
        anchors.fill: parent
        anchors.margins: Theme.panelInset
        visible: center.selectedPanel !== null
        color: Theme.surface
        border.color: Theme.line
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
            ToolButton {
                objectName: "panelClose"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                width: Theme.panelCloseSize
                height: Theme.panelCloseSize
                text: qsTr("✕")
                ToolTip.text: qsTr("닫기")
                Accessible.name: qsTr("닫기")
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
            onLoaded: item.onArg(shellController.panelArg)
        }
        Connections {
            target: shellController
            function onPanelArgRequested(id, arg) {
                if (id === shellController.activePanelId && loader.item) loader.item.onArg(arg)
            }
        }
    }
}
