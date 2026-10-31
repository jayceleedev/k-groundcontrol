// [0.1.0 T9 T10 T11 T12 T13 T16 T17 T20] specs/features/0.1.0-link.md
// 번역 문구를 한 곳에서 조립해 계기와 개요가 같은 이름을 쓰게 해요.
import QtQuick
import Kgc.VehicleInfo
QtObject {
    readonly property string flightMode: !VehicleInfoController.hasVehicle ? "—"
        : VehicleInfoController.modeName || qsTr("모드 %1").arg(VehicleInfoController.modeId)
    readonly property string gpsFix: {
        const fix = VehicleInfoController.fixType
        if (fix < 0) return "—"
        if (fix < 2) return qsTr("없음")
        if (fix === 2) return "2D"
        if (fix === 4) return "DGPS"
        if (fix === 5) return "RTK Float"
        if (fix === 6) return "RTK Fixed"
        return "3D"
    }
    function voltage(value: real): string { return isFinite(value) ? qsTr("%1 V").arg(value.toFixed(1)) : "—" }
    function current(value: real): string { return isFinite(value) ? qsTr("%1 A").arg(value.toFixed(1)) : "—" }
    function altitude(value: real): string { return isFinite(value) ? qsTr("%1 m").arg(value.toFixed(1)) : "—" }
    function speed(value: real): string { return isFinite(value) ? qsTr("%1 m/s").arg(value.toFixed(1)) : "—" }
    function number(value: real, digits: int): string {
        return isFinite(value) ? value.toFixed(digits) : "—"
    }
}
