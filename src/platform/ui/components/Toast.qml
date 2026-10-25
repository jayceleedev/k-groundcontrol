// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 짧은 알림의 모양을 제공해요.
 * 입력: text를 받아요. 표시 시점과 수명은 호출자가 정해요.
 * 출력: ink 바탕과 inkInverse 글자로 짧은 알림 모양을 제공해요.
 * 실패: 자동으로 닫히지 않으므로 호출자가 visible과 수명을 관리해야 해요.
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
