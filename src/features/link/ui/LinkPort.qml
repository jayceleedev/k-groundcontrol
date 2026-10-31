// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
import QtQuick
import Kgc.Ui
import Kgc.Link
ComboField {
    id: field
    objectName: "topbar_link.port"
    implicitWidth: Theme.topbarFieldWidth
    topPadding: 0
    bottomPadding: 0
    model: LinkController.ports.map(port => port.portName
        ? (port.description ? "%1 (%2)".arg(port.portName).arg(port.description) : port.portName) : "UDP 14550")
    currentIndex: LinkController.portIndex
    // 목록 교체가 선택 위치를 초기화할 수 있어 교체가 끝난 뒤 이름에 맞춰 다시 묶어요.
    onModelChanged: Qt.callLater(function() {
        field.currentIndex = Qt.binding(function() { return LinkController.portIndex })
    })
    displayText: qsTr("포트: %1").arg(LinkController.ports[LinkController.portIndex]?.portName || "UDP 14550")
    enabled: LinkController.canConnect
    onActivated: index => LinkController.selectPort(index)
}
