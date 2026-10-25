// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 실수로 누르지 않도록 누른 시간을 확인해요.
 * 입력: text, holdMs를 받고 confirmed()를 한 번 내요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
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
        height: Theme.menuGap
        color: Theme.danger
    }
}
