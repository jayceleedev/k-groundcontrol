// [0.0.2 T11 T12] specs/features/0.0.2-theme.md
/** 실수로 누르지 않도록 누른 시간을 확인해요.
 * 입력: text, holdMs를 받고 confirmed()를 한 번 내요.
 * 출력: progress가 채워지고 holdMs를 채운 누름마다 confirmed를 한 번 내요.
 * 실패: 일찍 떼거나 비활성화하면 진행률을 비우고 confirmed를 내지 않아요.
 */
import QtQuick
import Kgc.Ui

ActionButton {
    id: control
    property int holdMs: uiSettingValues.holdToConfirmMs
    property real progress: 0
    signal confirmed()
    kind: "secondary"
    textColor: enabled ? Theme.danger : Theme.inkDisabled
    onPressedChanged: {
        if (pressed && enabled) { progress = 0; fill.restart(); deadline.restart() }
        else { deadline.stop(); fill.stop(); progress = 0 }
    }
    onEnabledChanged: if (!enabled) { deadline.stop(); fill.stop(); progress = 0 }
    TimerBox { id: deadline; interval: control.holdMs; onTriggered: if (control.pressed && control.enabled) control.confirmed() }
    NumberAnimation { id: fill; target: control; property: "progress"; from: 0; to: 1; duration: control.holdMs }
    Rectangle {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: parent.width * control.progress
        height: Theme.holdProgressHeight
        color: Theme.danger
    }
}
