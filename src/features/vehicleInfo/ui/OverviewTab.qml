// [0.0.4 T5 T5b T6b T7 T10] specs/features/0.0.4-righttabs.md
// 개요 탭: 기체가 없으니 묶음 다섯의 틀만 두고 값은 모두 "—"로 비워요(링크의 연결만 "없음").
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
        EmptyState {
            Layout.fillWidth: true
            title: qsTr("기체 없음")
            detail: qsTr("하단 왼쪽 \"기체 연결\"에서 연결해요.")
        }
        Section {
            Layout.fillWidth: true
            text: qsTr("상태")
            KeyValueRow { width: parent.width; label: qsTr("시동 · 모드"); value: "— · —" }
        }
        Section {
            Layout.fillWidth: true
            text: qsTr("배터리")
            KeyValueRow { width: parent.width; label: qsTr("잔량"); value: "—" }
            ProgressLine { width: parent.width; from: 0; to: 100; value: 0 }
            KeyValueRow { width: parent.width; label: qsTr("전압 · 전류"); value: "— · —" }
        }
        Section {
            Layout.fillWidth: true
            text: qsTr("위치 · GPS")
            KeyValueRow { width: parent.width; label: qsTr("고도(상대 / AMSL)"); value: "— / —" }
            KeyValueRow { width: parent.width; label: qsTr("속도(수평 / 수직)"); value: "— / —" }
            KeyValueRow { width: parent.width; label: qsTr("GPS"); value: "—" }
            KeyValueRow { width: parent.width; label: qsTr("홈까지 거리"); value: "—" }
        }
        Section {
            Layout.fillWidth: true
            text: qsTr("링크")
            KeyValueRow { width: parent.width; label: qsTr("연결"); value: qsTr("없음") }
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
