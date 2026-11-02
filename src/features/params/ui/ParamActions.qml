// [0.3.0 6.8] specs/features/0.3.0-params.md
/** 오른쪽 세로 단추 묶음이에요.
 * 입력: ParamsController의 대기값 수와 Job이에요.
 * 출력: 쓰기·파일·재부팅 요청 시그널이에요. 새로고침은 바로 컨트롤러를 불러요.
 * 실패: Job이 도는 동안 전부 비활성이에요.
 */
import QtQuick
import QtQuick.Layouts
import Kgc.Ui
import Kgc.Params

ColumnLayout {
    id: actions
    signal writeRequested()
    signal openRequested()
    signal saveRequested()
    signal rebootRequested()
    readonly property bool idle: ParamsController.job === null
    spacing: Theme.inset
    ActionButton {
        objectName: "paramsWrite"
        Layout.fillWidth: true
        text: qsTr("값 쓰기 (%1)").arg(ParamsController.pendingCount)
        enabled: actions.idle && ParamsController.pendingCount > 0
        onClicked: actions.writeRequested()
    }
    ActionButton {
        objectName: "paramsRefresh"
        Layout.fillWidth: true
        kind: "secondary"
        text: qsTr("새로고침")
        enabled: actions.idle
        onClicked: ParamsController.refresh()
    }
    ActionButton {
        objectName: "paramsOpen"
        Layout.fillWidth: true
        kind: "secondary"
        text: qsTr("파일 불러오기")
        enabled: actions.idle
        onClicked: actions.openRequested()
    }
    ActionButton {
        objectName: "paramsSave"
        Layout.fillWidth: true
        kind: "secondary"
        text: qsTr("파일로 저장")
        enabled: actions.idle
        onClicked: actions.saveRequested()
    }
    ActionButton {
        objectName: "paramsReboot"
        Layout.fillWidth: true
        kind: "secondary"
        text: qsTr("FC 재부팅")
        enabled: actions.idle
        onClicked: actions.rebootRequested()
    }
    Item { Layout.fillHeight: true }
}
