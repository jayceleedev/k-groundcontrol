// [0.0.2 T8 T14 T18] specs/features/0.0.2-theme.md
import QtQuick
import Kgc.Ui
GlassSurface {
    objectName: "rightBar"
    width: Theme.rightBarWidth
    solidColor: Theme.region
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
