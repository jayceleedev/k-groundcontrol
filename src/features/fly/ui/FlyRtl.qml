// [0.6.0 5.11.9] specs/features/0.6.0-fly.md
/** 상단바의 'RTL' 단추예요(길게 눌러 확인).
 * 입력: 누를 수 있는지 알려 주는 FlyController.canRtl예요.
 * 출력: 다 누르면 누르기 시작한 때의 기체로 FlyController.rtl를 불러 RTL을 시작해요.
 * 실패: 누르는 사이 활성 기체가 바뀌면 아무것도 보내지 않아요.
 * 흐름:
 * 1. FlyHoldButton — 누르는 순간 기체를 기억해요.
 * 2. FlyController.rtl — 그 기체가 아직 활성이면 FlyService로 보내요.
 */
import QtQuick
import Kgc.Fly

FlyHoldButton {
    objectName: "topbar_fly.rtl"
    text: qsTr("RTL")
    icon.source: "qrc:/kgc/icons/home.svg"
    enabled: FlyController.canRtl
    onConfirmed: FlyController.rtl(pressedSysid)
}
