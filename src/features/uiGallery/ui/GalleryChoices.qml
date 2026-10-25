// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
Card {
    title: qsTr("고르기")
    Flow {
        width: parent.width
        spacing: Theme.panelInset
        Pill { objectName: "gallery_Pill_selected"; text: qsTr("선택됨"); checked: true }
        Pill { objectName: "gallery_Pill"; text: qsTr("선택 안 됨") }
        PillGroup { objectName: "gallery_PillGroup"; model: [qsTr("가로"), qsTr("세로"), qsTr("높이")] }
        Toggle { objectName: "gallery_Toggle"; text: qsTr("켬"); checked: true }
        Toggle { objectName: "gallery_Toggle_off"; text: qsTr("끔") }
        Toggle { text: qsTr("비활성"); enabled: false }
        CheckRow { objectName: "gallery_CheckRow"; text: qsTr("고름"); checked: true }
        CheckRow { objectName: "gallery_CheckRow_partial"; text: qsTr("일부"); checkState: Qt.PartiallyChecked }
        CheckRow { text: qsTr("안 고름") }
        RadioRow { objectName: "gallery_RadioRow"; text: qsTr("첫째"); checked: true }
        RadioRow { text: qsTr("둘째") }
        ComboField { objectName: "gallery_ComboField"; label: qsTr("연결 포트"); model: [qsTr("자동"), qsTr("직접 선택")] }
    }
}
