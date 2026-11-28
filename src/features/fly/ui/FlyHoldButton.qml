// [0.6.0 5.11.9] specs/features/0.6.0-fly.md
/** 비행 단추가 함께 쓰는 길게 눌러 확인 단추예요.
 * 입력: 글 text, 위험 표시 danger(빨간 글), enabled, 길게 누르는 시간(HoldToConfirm의 ui.holdToConfirmMs)이에요.
 * 출력: 누르는 순간의 활성 기체 pressedSysid와 시동 상태 pressedArmed를 기억하고, 다 누르면 confirmed를 내요.
 *       누르는 순간 초점을 가져가 입력란에 적힌 값이 확정돼요(Enter 없이도 보이는 값이 가요).
 * 실패: 일찍 떼거나 흐려지면 confirmed를 내지 않아요(HoldToConfirm).
 */
import QtQuick
import Kgc.Ui
import Kgc.Fly

HoldToConfirm {
    property bool danger: false
    property int pressedSysid: -1
    property bool pressedArmed: false
    kind: "secondary"
    textColor: !enabled ? Theme.inkDisabled : danger ? Theme.danger : Theme.ink
    onPressedChanged: if (pressed) { pressedSysid = FlyController.sysid; pressedArmed = FlyController.armed; forceActiveFocus() }
}
