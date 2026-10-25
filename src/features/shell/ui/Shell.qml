// [0.0.2 T8 T14 T18] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls
import Kgc.Ui
ApplicationWindow {
    id: window
    objectName: "shellWindow"
    title: qsTr("K-GroundControl %1").arg(Qt.application.version)
    minimumWidth: Math.min(Theme.baseWidth, shellController.availableScreenSize.width)
    minimumHeight: Math.min(Theme.baseHeight, shellController.availableScreenSize.height)
    color: Theme.window
    palette.window: Theme.window
    palette.windowText: Theme.ink
    palette.base: Theme.surface
    palette.text: Theme.ink
    palette.button: Theme.surface
    palette.buttonText: Theme.ink
    palette.highlight: Theme.accent
    palette.highlightedText: Theme.onAccent
    palette.placeholderText: Theme.inkMuted
    font.family: Theme.fontFamily
    font.pixelSize: Theme.fontSize
    Component.onCompleted: {
        shellController.attachWindow(this)
        width = minimumWidth
        height = minimumHeight
    }
    onVisibleChanged: if (visible) shellController.windowShown(width, height)
    Binding {
        target: Theme
        property: "uiScale"
        value: Math.min(1, window.width / Theme.baseWidth, window.height / Theme.baseHeight)
    }
    CenterArea {
        id: center
        x: Theme.glass ? 0 : left.width
        y: Theme.glass ? 0 : top.height
        width: Theme.glass ? window.width : window.width - left.width - right.width
        height: Theme.glass ? window.height : window.height - top.height - bottom.height
        viewportLeft: Theme.glass ? left.width : 0
        viewportRight: Theme.glass ? right.width : 0
        viewportTop: Theme.glass ? top.height : 0
        viewportBottom: Theme.glass ? bottom.height : 0
    }
    TopBar { id: top; source: center.mapStage; anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right }
    BottomBar { id: bottom; source: center.mapStage; anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right }
    LeftMenu { id: left; source: center.mapStage; anchors.top: top.bottom; anchors.bottom: bottom.top; anchors.left: parent.left }
    RightBar { id: right; source: center.mapStage; anchors.top: top.bottom; anchors.bottom: bottom.top; anchors.right: parent.right }
}
