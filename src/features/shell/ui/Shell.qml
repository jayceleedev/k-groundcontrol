// [0.0.1 T4–T9] specs/features/0.0.1-shell.md
import QtQuick
import QtQuick.Controls
import Kgc.Ui
ApplicationWindow {
    id: window
    objectName: "shellWindow"
    title: qsTr("K-GroundControl %1").arg(Qt.application.version)
    minimumWidth: Math.min(Theme.baseWidth, shellController.availableScreenSize.width)
    minimumHeight: Math.min(Theme.baseHeight, shellController.availableScreenSize.height)
    color: Theme.background
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
    TopBar { id: top; anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right }
    BottomBar { id: bottom; anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right }
    LeftMenu { id: left; anchors.top: top.bottom; anchors.bottom: bottom.top; anchors.left: parent.left }
    RightBar { id: right; anchors.top: top.bottom; anchors.bottom: bottom.top; anchors.right: parent.right }
    CenterArea { anchors.top: top.bottom; anchors.bottom: bottom.top; anchors.left: left.right; anchors.right: right.left }
}
