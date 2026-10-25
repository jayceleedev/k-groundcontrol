// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 상태를 작은 점으로 보여줘요.
 * 입력: level은 ok, warn, danger, info, idle 중 하나예요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Rectangle {
    property string level: "idle"
    implicitWidth: Theme.inset
    implicitHeight: Theme.inset
    radius: height / 2
    color: level === "ok" ? Theme.ok : level === "warn" ? Theme.warn : level === "danger" ? Theme.danger : level === "info" ? Theme.info : Theme.idle
}
