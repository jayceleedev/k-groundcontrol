// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
import QtQuick
import Kgc.Ui
import Kgc.Link
ActionButton {
    objectName: "topbar_link.connect"
    kind: "secondary"
    icon.source: "qrc:/kgc/icons/plug-connected.svg"
    text: LinkController.canConnect ? qsTr("연결") : qsTr("끊기")
    onClicked: LinkController.toggleConnection()
}
