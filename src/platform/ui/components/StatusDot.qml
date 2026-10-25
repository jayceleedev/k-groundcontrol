// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 상태를 작은 점으로 보여줘요.
 * 입력: level은 ok, warn, danger, info, idle 중 하나예요.
 * 출력: 점의 color와 옅은 바탕의 softColor로 상태 색을 제공해요.
 * 실패: 등록되지 않은 level은 idle과 surfaceAlt 색 쌍을 써요.
 */
import QtQuick
import Kgc.Ui

Rectangle {
    id: dot
    property string level: "idle"
    QtObject {
        id: levelPalette
        readonly property var colors: {
            switch (dot.level) {
            case "ok": return [Theme.ok, Theme.okSoft]
            case "warn": return [Theme.warn, Theme.warnSoft]
            case "danger": return [Theme.danger, Theme.dangerSoft]
            case "info": return [Theme.info, Theme.infoSoft]
            default: return [Theme.idle, Theme.surfaceAlt]
            }
        }
    }
    readonly property color softColor: levelPalette.colors[1]
    implicitWidth: Theme.inset
    implicitHeight: Theme.inset
    radius: height / 2
    color: levelPalette.colors[0]
}
