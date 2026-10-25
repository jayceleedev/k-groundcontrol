// [0.0.2 T8 T14 T18] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls
import Kgc.Ui
GlassSurface {
    objectName: "topBar"
    height: Theme.topBarHeight
    solidColor: Theme.region
    IconButton {
        objectName: "appIcon"
        anchors.left: parent.left
        anchors.leftMargin: Theme.inset
        anchors.verticalCenter: parent.verticalCenter
        width: Theme.appIconSize
        height: Theme.appIconSize
        padding: Theme.appIconPadding
        hint: qsTr("정보")
        contentItem: Image { source: "qrc:/kgc/icons/app.svg"; fillMode: Image.PreserveAspectFit }
        onClicked: navigator.open("appSettings", "about")
    }
    Repeater { model: topbarSlot; delegate: Item {} }
}
