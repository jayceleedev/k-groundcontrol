// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 짧은 알림의 모양을 제공해요.
 * 입력: text를 받아요. 표시 시점과 수명은 호출자가 정해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.Label {
    padding: Theme.inset
    color: Theme.inkInverse
    font.pixelSize: Theme.fontSize
    background: Rectangle { color: Theme.ink; radius: Theme.radiusMedium }
}
