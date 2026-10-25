// [0.0.2 T10] specs/features/0.0.2-theme.md
/** 진행률이나 기다리는 상태를 막대로 보여줘요.
 * 입력: from, to, value, indeterminate를 받아요.
 * 출력: value의 진행률 또는 indeterminate의 반복 이동 막대를 그려요.
 * 실패: 정해진 진행률은 from~to로 제한하고 보이지 않을 때 반복 이동을 멈춰요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.ProgressBar {
    id: control
    implicitWidth: Theme.progressWidth
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
                NumberAnimation { from: -fill.width; to: control.width; duration: 1500 }
            }
        }
    }
}
