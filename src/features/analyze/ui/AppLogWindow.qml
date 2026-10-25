// [0.0.3 T3 T4 T5 T6 T12 T14] specs/features/0.0.3-windows.md
/** 최근 로그를 따라가거나 사용자가 고른 위치에 머물러요.
 * 입력: appLogController의 목록과 창 자리 저장 기능.
 * 출력: following은 토글과 사용자 스크롤이 공유하는 따라가기 상태예요.
 * 실패: 빈 목록이면 기록을 표시하지 않아요.
 */
import QtQuick
import QtQuick.Window
import Kgc.Ui
Window {
    id: root
    objectName: "appLogWindow"
    visible: false
    title: qsTr("앱 로그 — K-GroundControl")
    color: Theme.window
    property bool following: true
    onXChanged: saveTimer.restart()
    onYChanged: saveTimer.restart()
    onWidthChanged: saveTimer.restart()
    onHeightChanged: saveTimer.restart()
    // 닫기 이벤트를 받아도 객체는 컨트롤러가 보관하므로 종료 요청을 거부하지 않아요.
    onClosing: appLogController.savePlacement()
    TimerBox { id: saveTimer; interval: appLogController.saveDelayMs; onTriggered: appLogController.savePlacement() }
    Toggle {
        id: followToggle
        objectName: "followToggle"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: Theme.inset
        text: qsTr("따라가기")
        checked: root.following
        onToggled: { root.following = checked; if (checked) logView.positionAtEnd() }
    }
    LogView {
        id: logView
        objectName: "appLogView"
        anchors.top: followToggle.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme.inset
        model: appLogController.model
        followTail: root.following
        onUserScrollStarted: root.following = false
        onUserScrolled: atEnd => root.following = atEnd
    }
}
