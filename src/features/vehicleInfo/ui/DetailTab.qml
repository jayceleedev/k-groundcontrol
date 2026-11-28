// [0.0.4 T5a T5b T6 T6a T6b T10] specs/features/0.0.4-righttabs.md
// 상세 탭: 흐린 찾기 칸과, 남은 칸을 채우는 빈 상태 안내만 둬요(값 묶음은 0.1.x).
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
        SearchBox { Layout.fillWidth: true; enabled: false; placeholderText: qsTr("값 찾기") }
        EmptyState {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("기체 없음")
            detail: qsTr("기체 연결 시 묶음별 값 표시")
        }
    }
}
