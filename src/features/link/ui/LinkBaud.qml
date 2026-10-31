// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
import QtQuick
import Kgc.Ui
import Kgc.Link
ComboField {
    objectName: "topbar_link.baud"
    implicitWidth: Theme.topbarFieldWidth
    topPadding: 0
    bottomPadding: 0
    model: LinkController.baudRates
    currentIndex: LinkController.baudRates.indexOf(LinkController.baudRate)
    displayText: qsTr("속도: %1").arg(LinkController.baudRate)
    enabled: !LinkController.udp && (LinkController.canConnect)
    onActivated: index => LinkController.selectBaud(index)
}
