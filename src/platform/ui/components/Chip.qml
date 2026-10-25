// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 상태 이름과 점을 함께 보여줘요.
 * 입력: text, level, radius를 받고 textColor로 글자 색을 읽어요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.Control {
    id: control
    property string text
    property real radius: height / 2
    property string level: "idle"
    readonly property color textColor: dot.color
    padding: Theme.inset
    implicitWidth: contentItem.implicitWidth + padding * 2
    implicitHeight: Theme.menuHeight
    contentItem: Row {
        spacing: Theme.inset
        StatusDot { id: dot; level: control.level; anchors.verticalCenter: parent.verticalCenter }
        Text { text: control.text; color: control.textColor; font.pixelSize: Theme.fontSize; anchors.verticalCenter: parent.verticalCenter }
    }
    background: Rectangle {
        radius: control.radius
        color: control.level === "ok" ? Theme.okSoft : control.level === "warn" ? Theme.warnSoft : control.level === "danger" ? Theme.dangerSoft : control.level === "info" ? Theme.infoSoft : Theme.surfaceAlt
        border.color: Theme.line
    }
}
