// [0.4.1 6.4 T13] specs/features/0.4.1-board-match.md
// [0.2.0 T14–T23 T26] specs/features/0.2.0-fcflash.md
/** 보드 확인부터 재연결 결과까지 네 단계를 보여요.
 * 입력: 서비스의 단계와 유리 바탕이에요.
 * 출력: 현재 단계의 화면이에요.
 * 실패: 각 단계가 서비스의 실패 문구를 보여요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import Kgc.FcFlash

Basic.ScrollView {
    id: panel
    objectName: "fcFlashPanel"
    property Item glassSource: null
    contentWidth: availableWidth
    rightPadding: Theme.scrollGutter
    Basic.ScrollBar.vertical.implicitWidth: Theme.scrollBarWidth
    clip: true
    /** 메뉴에서 열었을 때 초점을 받아요. 입력: 탐색 인자예요. 출력: 초점이에요. 실패: 없어요. */
    function onArg(arg: var) { forceActiveFocus() }
    Column {
        width: Math.min(panel.availableWidth, 760 * Theme.uiScale)
        // 패널 전체 폭의 가운데에 두되, 오른쪽 스크롤 여백 때문에 가로 스크롤이 생기지 않게 안쪽 폭에서 멈춰요.
        x: Math.max(0, Math.min((panel.width - width) / 2 - panel.leftPadding, panel.availableWidth - width))
        spacing: Theme.panelInset
        FlashStepBar { width: parent.width; currentStep: FcFlashController.step }
        StepBoard { width: parent.width; visible: FcFlashController.step === FlashStates.Board }
        StepFirmware { width: parent.width; visible: FcFlashController.step === FlashStates.Firmware }
        StepConfirm { width: parent.width; visible: FcFlashController.step === FlashStates.Confirm }
        StepFlash { width: parent.width; visible: FcFlashController.step === FlashStates.Flash }
    }
}
