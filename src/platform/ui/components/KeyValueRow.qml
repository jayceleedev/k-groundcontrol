// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 이름과 값을 한 줄에서 구분해요.
 * 입력: label과 value를 받아요.
 * 출력: label과 value를 줄 양쪽에 서로 다른 글자 색으로 표시해요.
 * 실패: 긴 글자를 줄이거나 검증하지 않으므로 호출자가 충분한 너비를 줘야 해요.
 */
import QtQuick
import Kgc.Ui

Item {
    id: row
    property string label: ""
    property string value: ""
    implicitWidth: Theme.keyValueWidth
    implicitHeight: Theme.controlHeight
    Text { text: row.label; color: Theme.inkMuted; anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: Theme.fontSize }
    Text { text: row.value; color: Theme.ink; anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: Theme.fontSize }
    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: Theme.lineWidth; color: Theme.line }
}
