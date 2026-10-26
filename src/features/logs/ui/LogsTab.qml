// [0.0.4 T5a T5b T6 T6b T10] specs/features/0.0.4-righttabs.md
// 로그 탭: 텔레메트리 기록과 온보드 로그의 틀만 두고, 단추는 모두 흐리게 해요(동작은 0.1.x).
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
        Section {
            Layout.fillWidth: true
            text: qsTr("텔레메트리 기록(tlog)")
            KeyValueRow { width: parent.width; label: qsTr("상태"); value: qsTr("기록 안 함(기체 없음)") }
            ActionButton { kind: "secondary"; enabled: false; text: qsTr("폴더 열기") }
        }
        Section {
            Layout.fillWidth: true
            text: qsTr("온보드 로그")
            ActionButton { kind: "secondary"; enabled: false; text: qsTr("목록 읽기") }
        }
        EmptyState {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("기체가 연결되면 목록을 읽어요")
        }
        Row {
            spacing: Theme.choiceGap
            ActionButton { kind: "secondary"; enabled: false; text: qsTr("내려받기") }
            ActionButton { kind: "secondary"; enabled: false; text: qsTr("모두 지우기") }
        }
    }
}
