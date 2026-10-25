// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 상태 이름과 점을 함께 보여줘요.
 * 입력: text, level, radius를 받아요.
 * 출력: StatusDot에서 얻은 textColor와 옅은 바탕으로 상태 글자를 표시하고 textColor로 글자 색을 제공해요.
 * 실패: 모르는 level은 StatusDot의 idle 색과 surfaceAlt 바탕으로 표시해요.
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
    implicitHeight: Theme.controlHeight
    contentItem: Row {
        spacing: Theme.inset
        StatusDot { id: dot; level: control.level; anchors.verticalCenter: parent.verticalCenter }
        Text { text: control.text; color: control.textColor; font.pixelSize: Theme.fontSize; anchors.verticalCenter: parent.verticalCenter }
    }
    background: Rectangle {
        radius: control.radius
        color: dot.softColor
        border.color: Theme.line
    }
}
