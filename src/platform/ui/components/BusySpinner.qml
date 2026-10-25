// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 기다리는 중임을 회전으로 보여줘요.
 * 입력: running을 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Item {
    id: control
    property bool running: true
    implicitWidth: Theme.iconSize
    implicitHeight: Theme.iconSize
    Rectangle { anchors.fill: parent; radius: height / 2; color: "transparent"; border.color: Theme.line; border.width: Theme.lineWidth * 2 }
    Rectangle { width: Theme.inset; height: Theme.menuGap; radius: height / 2; color: Theme.accent; anchors.horizontalCenter: parent.horizontalCenter }
    RotationAnimator on rotation { from: 0; to: 360; duration: uiSettingValues.holdToConfirmMs; loops: Animation.Infinite; running: control.running && control.visible }
}
