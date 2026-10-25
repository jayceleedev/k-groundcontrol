// [0.0.3 T9 T12 T14] specs/features/0.0.3-windows.md
/** 선택한 배치에 따라 빈 정보 칸을 보여줘요.
 * 입력: secondPanelController의 배치와 비율 사각형 목록.
 * 출력: 맞닿은 변에서만 여백의 절반을 뺀 칸들이 표시돼요.
 * 실패: 아직 정보 부품은 넣지 않아요.
 */
import QtQuick
import QtQuick.Window
import Kgc.Ui
Window {
    id: root
    objectName: "secondPanelWindow"
    visible: false
    title: qsTr("세컨드 패널 — K-GroundControl")
    color: Theme.window
    onXChanged: saveTimer.restart()
    onYChanged: saveTimer.restart()
    onWidthChanged: saveTimer.restart()
    onHeightChanged: saveTimer.restart()
    // 닫기 이벤트를 받아도 객체는 컨트롤러가 보관하므로 종료 요청을 거부하지 않아요.
    onClosing: secondPanelController.savePlacement()
    TimerBox { id: saveTimer; interval: secondPanelController.saveDelayMs; onTriggered: secondPanelController.savePlacement() }
    PillGroup {
        id: arrangementPicker
        objectName: "arrangementPicker"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: Theme.inset
        model: [qsTr("1칸"), qsTr("2칸 좌우"), qsTr("2칸 위아래"), qsTr("3칸(큰 칸 + 둘)"), qsTr("4칸"), qsTr("6칸 가로"), qsTr("6칸 세로")]
        currentIndex: secondPanelController.arrangementIndex
        onActivated: index => secondPanelController.chooseArrangement(index)
    }
    Item {
        anchors.top: arrangementPicker.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme.inset
        Repeater {
            model: secondPanelController.cells
            delegate: Rectangle {
                required property int index
                required property rect modelData
                readonly property real leftInset: modelData.x > 0 ? Theme.inset / 2 : 0
                readonly property real topInset: modelData.y > 0 ? Theme.inset / 2 : 0
                readonly property real rightInset: modelData.x + modelData.width < 1 ? Theme.inset / 2 : 0
                readonly property real bottomInset: modelData.y + modelData.height < 1 ? Theme.inset / 2 : 0
                objectName: "secondPanelCell_" + (index + 1)
                x: modelData.x * parent.width + leftInset
                y: modelData.y * parent.height + topInset
                width: modelData.width * parent.width - leftInset - rightInset
                height: modelData.height * parent.height - topInset - bottomInset
                color: Theme.surfaceAlt
                Text {
                    anchors.centerIn: parent
                    text: qsTr("칸 %1").arg(index + 1)
                    color: Theme.inkMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSize
                }
            }
        }
    }
}
