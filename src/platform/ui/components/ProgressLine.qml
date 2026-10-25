// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 진행률이나 기다리는 상태를 막대로 보여줘요.
 * 입력: from, to, value, indeterminate를 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.ProgressBar {
    id: control
    implicitWidth: Theme.languageWidth
    implicitHeight: Theme.inset
    background: Rectangle { color: Theme.line; radius: height / 2 }
    contentItem: Item {
        clip: true
        Rectangle {
            id: fill
            width: control.indeterminate ? parent.width / 3 : parent.width * control.position
            height: parent.height
            radius: height / 2
            color: Theme.accent
            SequentialAnimation on x {
                running: control.indeterminate && control.visible
                loops: Animation.Infinite
                NumberAnimation { from: -fill.width; to: control.width; duration: uiSettingValues.holdToConfirmMs }
            }
        }
    }
}
