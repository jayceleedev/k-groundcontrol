// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 이름과 값을 한 줄에서 구분해요.
 * 입력: label과 value를 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Item {
    id: row
    property string label: ""
    property string value: ""
    implicitWidth: Theme.languageWidth
    implicitHeight: Theme.menuHeight
    Text { text: row.label; color: Theme.inkMuted; anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: Theme.fontSize }
    Text { text: row.value; color: Theme.ink; anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; font.pixelSize: Theme.fontSize }
    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: Theme.lineWidth; color: Theme.line }
}
