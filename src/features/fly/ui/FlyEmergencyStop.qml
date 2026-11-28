// [0.6.0 5.11.9] specs/features/0.6.0-fly.md
/** 상단바의 비상 정지 단추예요(길게 눌러 확인, 결정 6).
 * 입력: FlyController.canEmergencyStop(시동이 걸리고 공중이거나 착륙 상태를 모를 때 참)이에요.
 * 출력: 다 누르면 누르기 시작한 때의 기체로 FlyController.emergencyStop을 불러요. 마우스를 올리면 설명 풍선을 보여요.
 * 실패: 누르는 사이 활성 기체가 바뀌면 아무것도 보내지 않아요.
 * 흐름:
 * 1. FlyHoldButton — 누르는 순간 기체를 기억해요.
 * 2. FlyController.emergencyStop — FlyService가 진행 중 명령에 끼어들어 모터를 꺼요.
 */
import QtQuick
import Kgc.Ui
import Kgc.Fly

FlyHoldButton {
    id: stopButton
    objectName: "topbar_fly.emergencyStop"
    text: qsTr("비상 정지")
    icon.source: "qrc:/kgc/icons/hand-stop.svg"
    danger: true
    enabled: FlyController.canEmergencyStop
    onConfirmed: FlyController.emergencyStop(pressedSysid)
    // 흐린 단추에는 마우스 올림이 오지 않아 눌릴 수 있을 때만 보여요.
    HintTip { objectName: "flyEmergencyStopTip"; text: qsTr("모터를 바로 꺼요. 공중이면 떨어져요."); visible: stopButton.hovered }
}
