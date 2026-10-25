// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
import QtQuick
import Kgc.Ui
Card {
    id: section
    objectName: "gallery_Card"
    title: qsTr("묶음")
    Section { objectName: "gallery_Section"; width: parent.width; text: qsTr("센서") }
    SettingsPage {
        id: settingsSample
        objectName: "gallery_SettingsPage"
        width: parent.width
        height: Theme.controlHeight * 5
        categories: [
            {id: "general", title: qsTr("일반")},
            {id: "display", title: qsTr("화면")}
        ]
        Section {
            width: parent.width
            category: "general"
            TextBox { placeholderText: qsTr("이름") }
        }
        Section {
            width: parent.width
            category: "display"
            Toggle { text: qsTr("표시"); checked: true }
        }
    }
    TabStrip { objectName: "gallery_TabStrip"; model: [qsTr("계기"), qsTr("개요"), qsTr("메시지")] }
    DataTable {
        objectName: "gallery_DataTable"
        width: parent.width
        columns: [qsTr("이름"), qsTr("값"), qsTr("단위")]
        rows: [[qsTr("배터리 전압"), "4.20", qsTr("V")], [qsTr("셀 수"), "4", ""], [qsTr("최대 속도"), "12.0", qsTr("m/s")]]
        currentIndex: 1
        onSortRequested: (column, ascending) => {
            rows = rows.slice().sort((a, b) => String(a[column]).localeCompare(String(b[column])) * (ascending ? 1 : -1))
        }
    }
    LogView {
        objectName: "gallery_LogView"
        width: parent.width
        model: [{text: qsTr("포트 열기 시도"), level: "debug"}, {text: qsTr("연결 완료"), level: "info"},
                {text: qsTr("위성 수 부족"), level: "warn"}, {text: qsTr("보정 실패: 흔들림"), level: "error"}]
    }
    GlassSurface {
        objectName: "gallery_GlassSurface"
        source: section.source
        width: parent.width
        height: Theme.menuHeight * 2
        radius: Theme.radiusMedium
        Text { anchors.centerIn: parent; text: qsTr("에어로에서는 뒤가 흐리게 비쳐요"); color: Theme.ink; font.pixelSize: Theme.fontSize }
    }
}
