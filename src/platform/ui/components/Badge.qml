// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 알림 개수를 둥근 표식으로 보여줘요.
 * 입력: value를 받아요.
 * 출력: value를 위험 색 원형 바탕 위 숫자로 표시해요.
 * 실패: 음수도 받은 정수 그대로 표시하므로 개수 유효성은 호출자가 확인해요.
 */
import QtQuick
import Kgc.Ui

Rectangle {
    id: badge
    property int value: 0
    implicitWidth: Math.max(Theme.iconSize, label.implicitWidth + Theme.inset)
    implicitHeight: Theme.iconSize
    radius: height / 2
    color: Theme.danger
    Text { id: label; anchors.centerIn: parent; text: badge.value; color: Theme.dangerInk; font.pixelSize: Theme.fontSize }
}
