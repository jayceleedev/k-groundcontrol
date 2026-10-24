// [0.0.1 T1] specs/features/0.0.1-shell.md
pragma Singleton
import QtQuick
QtObject {
    readonly property int baseWidth: 1920
    readonly property int baseHeight: 900
    property real uiScale: 1
    readonly property int topBarHeight: Math.round(48 * uiScale)
    readonly property int leftMenuWidth: Math.round(200 * uiScale)
    readonly property int leftMenuCollapsedWidth: Math.round(56 * uiScale)
    readonly property int rightBarWidth: Math.round(280 * uiScale)
    readonly property int bottomBarHeight: Math.round(32 * uiScale)
    readonly property int instrumentHeight: Math.round(240 * uiScale)
    readonly property int inset: Math.round(8 * uiScale)
    readonly property int panelInset: Math.round(16 * uiScale)
    readonly property int menuGap: Math.round(3 * uiScale)
    readonly property int menuHeight: Math.round(36 * uiScale)
    readonly property int iconSize: Math.round(20 * uiScale)
    readonly property int appIconSize: Math.round(40 * uiScale)
    readonly property int lineWidth: 1
    readonly property int zero: 0
    readonly property int fontSize: Math.round(13 * uiScale)
    readonly property int titleSize: Math.round(14 * uiScale)
    readonly property int dialogWidth: Math.round(720 * uiScale)
    readonly property int dialogHeight: Math.round(560 * uiScale)
    readonly property int languageWidth: Math.round(240 * uiScale)
    readonly property color background: "#f3f4f6"
    readonly property color region: "#e5e7eb"
    readonly property color surface: "#ffffff"
    readonly property color line: "#9ca3af"
    readonly property color ink: "#111827"
    readonly property color dim: "#6b7280"
    readonly property color accent: "#374151"
}
