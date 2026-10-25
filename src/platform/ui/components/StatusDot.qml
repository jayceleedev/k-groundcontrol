// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 상태를 작은 점으로 보여줘요.
 * 입력: level은 ok, warn, danger, info, idle 중 하나예요. levelColors·color·softColor로 상태 색을 읽어요.
 * 출력: levelColors의 앞 색을 점의 color로, 뒤 색을 softColor로 제공해요.
 * 실패: 등록되지 않은 level은 idle과 surfaceAlt 색 쌍을 써요.
 */
import QtQuick
import Kgc.Ui

Rectangle {
    property string level: "idle"
    readonly property var levelColors: {
        switch (level) {
        case "ok": return [Theme.ok, Theme.okSoft]
        case "warn": return [Theme.warn, Theme.warnSoft]
        case "danger": return [Theme.danger, Theme.dangerSoft]
        case "info": return [Theme.info, Theme.infoSoft]
        default: return [Theme.idle, Theme.surfaceAlt]
        }
    }
    readonly property color softColor: levelColors[1]
    implicitWidth: Theme.inset
    implicitHeight: Theme.inset
    radius: height / 2
    color: levelColors[0]
}
