// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 닫을 수 있는 상태 알림을 보여줘요.
 * 입력: text, level과 closed()를 사용해요.
 * 출력: 닫기 단추가 visible을 거짓으로 바꾸고 closed를 내요.
 * 실패: 모르는 level의 색은 바탕 부품 Chip의 기본 상태 표시를 따라요.
 */
import QtQuick
import Kgc.Ui

Chip {
    id: control
    signal closed()
    implicitWidth: contentItem.implicitWidth + padding * 2 + Theme.flatIconButtonSize + Theme.inset
    rightPadding: Theme.flatIconButtonSize + Theme.inset * 2
    radius: Theme.radiusMedium
    IconButton { flat: true; icon.color: control.textColor; anchors.right: parent.right; anchors.rightMargin: Theme.inset; anchors.verticalCenter: parent.verticalCenter; name: "x"; hint: qsTr("닫기"); onClicked: { control.visible = false; control.closed() } }
}
