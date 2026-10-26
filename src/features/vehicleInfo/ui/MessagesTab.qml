// [0.0.4 T5a T5b T6 T6a T6b T10] specs/features/0.0.4-righttabs.md
// 메시지 탭: 첫 줄 흐린 수준 필터, 둘째 줄 흐린 찾기, 아래는 남은 칸을 채우는 빈 상태 안내예요(목록은 0.1.x).
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
        PillGroup { enabled: false; model: [qsTr("전체"), qsTr("오류"), qsTr("경고"), qsTr("정보")] }
        SearchBox { Layout.fillWidth: true; enabled: false; placeholderText: qsTr("찾기") }
        EmptyState {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("아직 메시지가 없어요")
            detail: qsTr("기체가 보내는 상태 메시지가 여기에 쌓여요.")
        }
    }
}
