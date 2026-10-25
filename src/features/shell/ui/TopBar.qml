// [0.0.1 T4–T9] specs/features/0.0.1-shell.md
import QtQuick
import QtQuick.Controls
import Kgc.Ui
Rectangle {
    objectName: "topBar"
    height: Theme.topBarHeight
    color: Theme.region
    border.color: Theme.line
    ToolButton {
        objectName: "appIcon"
        anchors.left: parent.left
        anchors.leftMargin: Theme.inset
        anchors.verticalCenter: parent.verticalCenter
        width: Theme.appIconSize
        height: Theme.appIconSize
        padding: Theme.appIconPadding
        Accessible.name: qsTr("정보")
        contentItem: Image { source: "qrc:/kgc/icons/app.svg"; fillMode: Image.PreserveAspectFit }
        onClicked: navigator.open("appSettings", "about")
    }
    Repeater { model: topbarSlot; delegate: Item {} }
}
