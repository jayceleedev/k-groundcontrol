// [0.2.0 5.3] specs/features/0.2.0-fcflash.md
/** 작업 단계와 진행률, 남은 시간, 취소 요청을 한 곳에서 보여 줘요.
 * 입력: 작업 객체 job과 오류 글자를 보일지 정하는 showError예요.
 * 출력: 작업의 읽기 전용 값과 취소 단추를 보여 줘요.
 * 실패: 작업이 없으면 빈 표시로 남아요.
 */
import QtQuick
import QtQuick.Layouts
import Kgc.Ui

ColumnLayout {
    id: root
    property var job: null
    property bool showError: true
    spacing: Theme.inset

    Text {
        Layout.fillWidth: true
        text: root.job ? qsTranslate("Job", root.job.stepName) : ""
        color: Theme.ink
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        wrapMode: Text.Wrap
    }
    ProgressLine {
        Layout.fillWidth: true
        indeterminate: !root.job || root.job.progress < 0
        value: root.job ? root.job.progress : 0
    }
    Text {
        visible: root.job !== null && root.job.etaSeconds >= 0
        text: root.job ? qsTr("약 %1초 남음").arg(root.job.etaSeconds) : ""
        color: Theme.ink
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
    }
    ActionButton {
        visible: root.job !== null && root.job.cancellable
        text: qsTr("취소")
        kind: "secondary"
        // 취소는 상태를 바꾸지 않고 작업 소유자에게 요청만 알려요.
        onClicked: root.job.cancelRequested()
    }
    Text {
        Layout.fillWidth: true
        visible: root.showError && text !== ""
        text: root.job ? root.job.errorText : ""
        color: Theme.danger
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
        wrapMode: Text.Wrap
    }
}
