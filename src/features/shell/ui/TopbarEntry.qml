// [0.0.1 T17] specs/features/0.0.1-shell.md
/** 상단바 슬롯 한 항목을 그 영역에 그려요. 아직 기능이 없는 자리는 비활성 단추로 보여요.
 * 입력: area(이 줄이 그리는 영역)와 슬롯 모델의 id·area·labelKey·icon·iconOnly·separatorBefore.
 * 출력: 영역이 맞으면 앞 구분선과 단추를, 아니면 아무것도 보이지 않아요.
 * 실패: 없음.
 */
import QtQuick
import Kgc.Ui
Row {
    id: entry
    required property var model
    property string area
    visible: model.area === area
    spacing: Theme.inset
    height: Theme.controlHeight
    Rectangle {
        visible: entry.model.separatorBefore
        width: Theme.lineWidth
        height: Theme.frameButtonSize
        color: Theme.lineStrong
        anchors.verticalCenter: parent.verticalCenter
    }
    ActionButton {
        visible: !entry.model.iconOnly
        objectName: "topbar_" + entry.model.id
        kind: "secondary"
        enabled: false
        text: { translator.language; return qsTranslate("Bar", entry.model.labelKey) }
        icon.source: entry.model.icon ? "qrc:/kgc/icons/" + entry.model.icon + ".svg" : ""
        anchors.verticalCenter: parent.verticalCenter
    }
    IconButton {
        visible: entry.model.iconOnly
        objectName: "topbar_" + entry.model.id
        enabled: false
        name: entry.model.icon
        hint: { translator.language; return qsTranslate("Bar", entry.model.labelKey) }
        anchors.verticalCenter: parent.verticalCenter
    }
}
