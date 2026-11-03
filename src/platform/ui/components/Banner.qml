// [0.4.0 6.8] specs/features/0.4.0-sitl.md
// [0.0.3 T18] specs/features/0.0.3-windows.md
// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 닫을 수 있는 상태 알림을 보여줘요.
 * 입력: text, level, closable(기본 참)과 closed()를 사용해요. closable이 거짓이면 닫기 단추를 숨겨, 보이기는 호출자의 visible만 정해요.
 * 출력: 닫기 단추가 visible을 거짓으로 바꾸고 closed를 내요.
 * 실패: 모르는 level의 색은 바탕 부품 Chip의 기본 상태 표시를 따라요.
 */
import QtQuick
import Kgc.Ui

Chip {
    id: control
    property bool closable: true
    signal closed()
    implicitWidth: contentItem.implicitWidth + padding * 2 + (closable ? Theme.flatIconButtonSize + Theme.inset : 0)
    rightPadding: closable ? Theme.flatIconButtonSize + Theme.inset * 2 : padding
    radius: Theme.radiusMedium
    IconButton { visible: control.closable; flat: true; icon.color: control.textColor; anchors.right: parent.right; anchors.rightMargin: Theme.inset; anchors.verticalCenter: parent.verticalCenter; name: "x"; hint: qsTr("닫기"); onClicked: { control.visible = false; control.closed() } }
}
