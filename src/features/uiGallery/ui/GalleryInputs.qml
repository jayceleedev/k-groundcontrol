// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
Card {
    title: qsTr("입력")
    Flow {
        width: parent.width
        spacing: Theme.panelInset
        TextBox { objectName: "gallery_TextBox"; label: qsTr("기체 이름"); text: qsTr("기체 1") }
        TextBox { objectName: "gallery_TextBox_placeholder"; label: qsTr("이름"); placeholderText: qsTr("이름을 입력해 주세요") }
        TextBox { objectName: "gallery_TextBox_error"; label: qsTr("이름"); errorText: qsTr("이름을 확인해 주세요") }
        NumberBox { objectName: "gallery_NumberBox"; value: 12.5; decimals: 1; stepSize: 0.5; unit: qsTr("m/s") }
        NumberBox { objectName: "gallery_NumberBox_error"; value: 250; from: 0; to: 120; unit: qsTr("m") }
        SearchBox { objectName: "gallery_SearchBox"; placeholderText: qsTr("검색"); text: qsTr("배터리") }
        SliderField { objectName: "gallery_SliderField"; from: 0; to: 100; value: 62; stepSize: 1; unit: qsTr("%") }
        FileField { objectName: "gallery_FileField"; text: qsTr("설정 파일"); onBrowseRequested: text = qsTr("견본 파일을 선택했어요") }
    }
}
