// [0.1.0 T9 T10 T11 T12 T13 T16 T17 T20] specs/features/0.1.0-link.md
// [0.0.4 T5 T5b T6b T7 T10] specs/features/0.0.4-righttabs.md
// 마지막 수신값을 읽어 응답 유실 중에도 개요를 확인할 수 있게 해요.
import QtQuick
import QtQuick.Layouts
import Kgc.Ui
import Kgc.VehicleInfo
Item {
    VehicleInfoValues { id: values }
    implicitHeight: layout.implicitHeight + Theme.inset * 2
    ColumnLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: Theme.inset
        spacing: Theme.inset
        EmptyState {
            Layout.fillWidth: true
            visible: !VehicleInfoController.hasVehicle
            title: qsTr("기체 없음")
            detail: qsTr("상단바에서 포트를 고르고 연결해요.")
        }
        Section {
            Layout.fillWidth: true
            text: qsTr("상태")
            KeyValueRow { width: parent.width; label: qsTr("시동 · 모드"); value: !VehicleInfoController.hasVehicle ? "— · —" : qsTr("%1 · %2").arg(VehicleInfoController.armed ? qsTr("시동") : qsTr("해제")).arg(values.flightMode) }
        }
        Section {
            Layout.fillWidth: true
            text: qsTr("배터리")
            KeyValueRow { width: parent.width; label: qsTr("잔량"); value: isFinite(VehicleInfoController.batteryPercent) ? qsTr("%1%").arg(VehicleInfoController.batteryPercent) : "—" }
            ProgressLine { width: parent.width; from: 0; to: 100; value: isFinite(VehicleInfoController.batteryPercent) ? VehicleInfoController.batteryPercent : 0 }
            KeyValueRow { width: parent.width; label: qsTr("전압 · 전류"); value: qsTr("%1 · %2").arg(values.voltage(VehicleInfoController.batteryVoltage)).arg(values.current(VehicleInfoController.batteryCurrent)) }
        }
        Section {
            Layout.fillWidth: true
            text: qsTr("위치 · GPS")
            KeyValueRow { width: parent.width; label: qsTr("고도(상대 / AMSL)"); value: qsTr("%1 / %2").arg(values.altitude(VehicleInfoController.altitude)).arg(values.altitude(VehicleInfoController.altAmsl)) }
            KeyValueRow { width: parent.width; label: qsTr("속도(수평 / 수직)"); value: qsTr("%1 / %2").arg(values.speed(VehicleInfoController.speed)).arg(values.speed(VehicleInfoController.climbRate)) }
            KeyValueRow { width: parent.width; label: qsTr("GPS"); value: VehicleInfoController.fixType >= 0 ? qsTr("%1 · 위성 %2").arg(values.gpsFix).arg(values.number(VehicleInfoController.satellites, 0)) : "—" }
            KeyValueRow { width: parent.width; label: qsTr("홈까지 거리"); value: "—" }
        }
        Section {
            Layout.fillWidth: true
            text: qsTr("링크")
            KeyValueRow { width: parent.width; label: qsTr("연결"); value: !VehicleInfoController.hasVehicle ? qsTr("없음") : VehicleInfoController.udp ? "UDP 14550" : qsTr("%1 @ %2").arg(VehicleInfoController.portName).arg(VehicleInfoController.baudRate) }
        }
        Section {
            Layout.fillWidth: true
            text: qsTr("비행")
            KeyValueRow { width: parent.width; label: qsTr("비행 시간 · 거리"); value: "— · —" }
        }
        // 칸이 내용보다 길면 남는 자리를 맨 아래로 모아 묶음들이 위에서부터 놓이게 해요.
        Item { Layout.fillHeight: true }
    }
}
