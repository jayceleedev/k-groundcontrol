// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
Card {
    title: qsTr("표시")
    Flow {
        width: parent.width
        spacing: Theme.panelInset
        Chip { objectName: "gallery_Chip_ok"; text: qsTr("정상"); level: "ok" }
        Chip { objectName: "gallery_Chip_warn"; text: qsTr("주의"); level: "warn" }
        Chip { objectName: "gallery_Chip_danger"; text: qsTr("위험"); level: "danger" }
        Chip { objectName: "gallery_Chip_info"; text: qsTr("정보"); level: "info" }
        Chip { objectName: "gallery_Chip_idle"; text: qsTr("없음"); level: "idle" }
        Badge { objectName: "gallery_Badge"; value: 3 }
        StatusDot { objectName: "gallery_StatusDot"; level: "ok" }
        ValueTile { objectName: "gallery_ValueTile"; value: "15.8"; unit: qsTr("V"); label: qsTr("배터리") }
        KeyValueRow { objectName: "gallery_KeyValueRow"; label: qsTr("연결 상태"); value: qsTr("연결됨") }
        LevelBar { objectName: "gallery_LevelBar"; value: 56; minimum: -80; maximum: 90 }
        ProgressLine { objectName: "gallery_ProgressLine"; value: 0.45 }
        ProgressLine { objectName: "gallery_ProgressLine_busy"; indeterminate: true }
        BusySpinner { objectName: "gallery_BusySpinner" }
        StepList {
            objectName: "gallery_StepList"
            model: [{text: qsTr("수평"), state: "done"}, {text: qsTr("왼쪽 옆"), state: "current"},
                    {text: qsTr("오른쪽 옆"), state: "pending"}, {text: qsTr("뒤집기 실패"), state: "failed"}]
        }
    }
    Flow {
        width: parent.width
        spacing: Theme.panelInset
        Repeater {
            model: ["fcFlash", "layout", "sensors", "radio", "flightModes", "power", "failsafe", "motors", "tuning", "logs", "esc", "elrs", "tracker", "params", "appSettings", "uiGallery"]
            IconGlyph { required property string modelData; objectName: "gallery_IconGlyph_" + modelData; name: "menu/" + modelData }
        }
    }
}
