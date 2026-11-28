// [0.6.0 5.11.9] specs/features/0.6.0-fly.md
/** 상단바의 시동·시동 끄기 단추예요(길게 눌러 확인).
 * 입력: FlyController의 armed·canArm·canDisarm이에요.
 * 출력: 시동이 꺼져 있으면 '시동', 걸려 있으면 '시동 끄기'(빨간 글)를 보이고, 다 누르면 arm·disarm을 불러요.
 * 실패: 누르기 시작한 때와 다 누른 때의 시동 상태가 다르면 아무것도 보내지 않아요(기체가 스스로 시동을 바꾼 경우).
 * 흐름:
 * 1. FlyHoldButton — 누르는 순간 기체와 시동 상태를 기억해요.
 * 2. FlyController.arm·disarm — 그 기체가 아직 활성이면 FlyService로 보내요.
 */
import QtQuick
import Kgc.Fly

FlyHoldButton {
    objectName: "topbar_fly.arm"
    text: FlyController.armed ? qsTr("시동 끄기") : qsTr("시동")
    danger: FlyController.armed
    enabled: FlyController.armed ? FlyController.canDisarm : FlyController.canArm
    onConfirmed: if (FlyController.armed === pressedArmed) { if (pressedArmed) FlyController.disarm(pressedSysid); else FlyController.arm(pressedSysid) }
}
