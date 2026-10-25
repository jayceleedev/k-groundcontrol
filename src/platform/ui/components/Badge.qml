// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 알림 개수를 둥근 표식으로 보여줘요.
 * 입력: value를 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Rectangle {
    id: badge
    property int value: 0
    implicitWidth: Math.max(Theme.iconSize, label.implicitWidth + Theme.inset)
    implicitHeight: Theme.iconSize
    radius: height / 2
    color: Theme.danger
    Text { id: label; anchors.centerIn: parent; text: badge.value; color: Theme.onDanger; font.pixelSize: Theme.fontSize }
}
