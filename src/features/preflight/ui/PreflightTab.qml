// [0.0.4 T5a T5b T6 T6a T6b T10] specs/features/0.0.4-righttabs.md
// 점검 탭: 점검표는 기체 종류로 정해지므로 기체가 없으면 흐린 점검표 고르기와 빈 상태 안내만 보여요.
import QtQuick
import QtQuick.Layouts
import Kgc.Ui
Item {
    implicitHeight: layout.implicitHeight + Theme.inset * 2
    ColumnLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: Theme.inset
        spacing: Theme.inset
        ComboField { Layout.fillWidth: true; enabled: false; label: qsTr("점검표"); displayText: "—" }
        EmptyState {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("기체 없음")
            detail: qsTr("기체 연결 시 기체 종류에 맞는 점검표 표시")
        }
    }
}
