// [0.0.1 T4–T9] specs/features/0.0.1-shell.md
import QtQuick
import Kgc.Ui
Rectangle {
    objectName: "rightBar"
    width: Theme.rightBarWidth
    color: Theme.region
    border.color: Theme.line
    Rectangle {
        id: instrument
        objectName: "instrumentPlaceholder"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme.inset
        height: Theme.instrumentHeight
        color: Theme.surface
        border.color: Theme.line
    }
    Rectangle {
        objectName: "rightTabs"
        anchors.top: instrument.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme.inset
        height: Theme.rightTabHeight
        color: Theme.surface
        border.color: Theme.line
        Repeater { model: rightTabSlot; delegate: Item {} }
    }
}
