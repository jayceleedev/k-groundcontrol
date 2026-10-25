// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
Card {
    title: qsTr("버튼")
    Flow {
        width: parent.width
        spacing: Theme.inset
        ActionButton { objectName: "gallery_ActionButton_primary"; text: qsTr("연결"); icon.source: "qrc:/kgc/icons/menu/radio.svg" }
        ActionButton { objectName: "gallery_ActionButton_secondary"; kind: "secondary"; text: qsTr("취소") }
        ActionButton { objectName: "gallery_ActionButton_ghost"; kind: "ghost"; text: qsTr("자세히") }
        ActionButton { objectName: "gallery_ActionButton_danger"; kind: "danger"; text: qsTr("초기화") }
        ActionButton { objectName: "gallery_ActionButton_disabled"; text: qsTr("비활성"); enabled: false }
        IconButton { objectName: "gallery_IconButton"; name: "menu/appSettings"; hint: qsTr("설정 열기") }
        HoldToConfirm { objectName: "gallery_HoldToConfirm"; text: qsTr("길게 눌러 확인"); onConfirmed: { confirmation.visible = true; hideConfirmation.restart() } }
        Text { id: confirmation; text: qsTr("확인됨"); visible: false; color: Theme.ok; font.pixelSize: Theme.fontSize }
    }
    TimerBox { id: hideConfirmation; interval: 2000; onTriggered: confirmation.visible = false }
}
