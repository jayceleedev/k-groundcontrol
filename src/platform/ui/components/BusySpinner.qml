// [0.0.2 T10] specs/features/0.0.2-theme.md
/** 기다리는 중임을 회전으로 보여줘요.
 * 입력: running을 받아요.
 * 출력: running인 동안 원 위 표식을 일정한 속도로 회전시켜요.
 * 실패: running이 거짓이거나 화면에 보이지 않으면 회전을 멈춰요.
 */
import QtQuick
import Kgc.Ui

Item {
    id: control
    property bool running: true
    implicitWidth: Theme.iconSize
    implicitHeight: Theme.iconSize
    Rectangle { anchors.fill: parent; radius: height / 2; color: "transparent"; border.color: Theme.line; border.width: Theme.lineWidth * 2 }
    Rectangle { width: Theme.inset; height: Theme.spinnerMarkHeight; radius: height / 2; color: Theme.accent; anchors.horizontalCenter: parent.horizontalCenter }
    RotationAnimator on rotation { from: 0; to: 360; duration: 1000; loops: Animation.Infinite; running: control.running && control.visible }
}
