// [0.0.1 T4–T9] specs/features/0.0.1-shell.md
import QtQuick
import QtQuick.Controls
import Kgc.Ui
Rectangle {
    id: menu
    objectName: "leftMenu"
    width: shellController.leftMenuCollapsed ? Theme.leftMenuCollapsedWidth : Theme.leftMenuWidth
    color: Theme.region
    border.color: Theme.line
    ToolButton {
        id: collapse
        objectName: "leftMenuCollapse"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: Theme.inset
        width: Theme.menuHeight
        height: Theme.menuHeight
        text: shellController.leftMenuCollapsed ? qsTr("▶") : qsTr("◀")
        onClicked: shellController.leftMenuCollapsed = !shellController.leftMenuCollapsed
    }
    Flickable {
        anchors.top: collapse.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme.inset
        contentHeight: entries.height
        clip: true
        Column {
            id: entries
            width: parent.width
            spacing: Theme.menuGap
            Repeater {
                model: panelRegistry
                delegate: Column {
                    id: entry
                    required property var model
                    property string panelId: model.id
                    property string menuTitle: {
                        const language = translator.language
                        return qsTranslate("Menu", model.titleKey)
                    }
                    width: entries.width
                    spacing: Theme.menuGap
                    Button {
                        objectName: "menuItem_" + entry.panelId
                        width: parent.width
                        height: Theme.menuHeight
                        padding: Theme.inset
                        text: entry.menuTitle
                        Accessible.name: entry.menuTitle
                        ToolTip.text: entry.menuTitle
                        ToolTip.visible: hovered && shellController.leftMenuCollapsed
                        onClicked: shellController.togglePanel(entry.panelId)
                        background: Rectangle {
                            color: shellController.activePanelId === entry.panelId ? Theme.accent : Theme.surface
                            border.color: Theme.line
                        }
                        contentItem: Row {
                            spacing: Theme.inset
                            Image {
                                width: Theme.iconSize
                                height: Theme.iconSize
                                anchors.verticalCenter: parent.verticalCenter
                                source: entry.model.icon
                                fillMode: Image.PreserveAspectFit
                            }
                            Text {
                                visible: !shellController.leftMenuCollapsed
                                anchors.verticalCenter: parent.verticalCenter
                                text: entry.menuTitle
                                font.pixelSize: Theme.fontSize
                                color: shellController.activePanelId === entry.panelId ? Theme.surface : Theme.ink
                            }
                        }
                    }
                    Rectangle {
                        objectName: entry.model.separatorAfter ? "menuSeparator_" + entry.model.group : ""
                        visible: entry.model.separatorAfter
                        width: parent.width
                        height: Theme.lineWidth
                        color: Theme.line
                    }
                }
            }
        }
    }
}
