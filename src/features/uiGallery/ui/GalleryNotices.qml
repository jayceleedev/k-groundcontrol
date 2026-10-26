// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
// [0.0.4 T11] specs/features/0.0.4-righttabs.md
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
Card {
    title: qsTr("알림")
    Banner { objectName: "gallery_Banner_warn"; text: qsTr("시동 중에는 펌웨어를 굽지 않아요"); level: "warn" }
    Banner { objectName: "gallery_Banner_info"; text: qsTr("새 펌웨어가 있어요"); level: "info" }
    Flow {
        width: parent.width
        spacing: Theme.panelInset
        Toast { objectName: "gallery_Toast"; text: qsTr("설정을 저장했어요") }
        ActionButton {
            id: tipAnchor
            kind: "secondary"
            text: qsTr("도움말")
            HintTip { objectName: "gallery_HintTip"; text: qsTr("설정 열기"); visible: tipAnchor.hovered }
        }
        ActionButton { kind: "danger"; text: qsTr("확인 창 열기"); onClicked: confirmDialog.open() }
        EmptyState { objectName: "gallery_EmptyState"; title: qsTr("아직 메시지가 없어요"); detail: qsTr("기체가 보내는 상태 메시지가 여기에 쌓여요.") }
    }
    ConfirmDialog {
        id: confirmDialog
        objectName: "gallery_ConfirmDialog"
        parent: Basic.Overlay.overlay
        anchors.centerIn: parent
        title: qsTr("설정을 초기화할까요?")
        text: qsTr("이 목록에서는 실제 설정을 바꾸지 않아요.")
        danger: true
        confirmText: qsTr("초기화")
    }
}
