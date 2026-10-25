// [0.0.1 T1] specs/features/0.0.1-shell.md
pragma Singleton
import QtQuick
import "themes"
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
    readonly property int appIconPadding: 0
    readonly property int rightTabHeight: Math.round(32 * uiScale)
    readonly property int panelHeaderHeight: Math.round(48 * uiScale)
    readonly property int panelCloseSize: Math.round(40 * uiScale)
    readonly property real lineWidth: 1 * uiScale
    readonly property int fontSize: Math.round(13 * uiScale)
    readonly property int titleSize: Math.round(14 * uiScale)
    readonly property int dialogWidth: Math.round(720 * uiScale)
    readonly property int dialogHeight: Math.round(560 * uiScale)
    readonly property int languageWidth: Math.round(240 * uiScale)
    property ThemeBase current: Light {}
    readonly property color series1: current.series1
    readonly property color series2: current.series2
    readonly property color series3: current.series3
    readonly property color series4: current.series4
    readonly property color series5: current.series5
    readonly property color series6: current.series6
    readonly property color chartGrid: current.chartGrid
    readonly property color chartAxis: current.chartAxis
    readonly property color onDanger: current.onDanger
    readonly property color window: current.window
    readonly property color region: current.region
    readonly property color surface: current.surface
    readonly property color surfaceAlt: current.surfaceAlt
    readonly property color overlay: current.overlay
    readonly property color ink: current.ink
    readonly property color inkMuted: current.inkMuted
    readonly property color inkInverse: current.inkInverse
    readonly property color inkDisabled: current.inkDisabled
    readonly property color line: current.line
    readonly property color lineStrong: current.lineStrong
    readonly property color focus: current.focus
    readonly property color accent: current.accent
    readonly property color accentHover: current.accentHover
    readonly property color accentPressed: current.accentPressed
    readonly property color onAccent: current.onAccent
    readonly property color ok: current.ok
    readonly property color warn: current.warn
    readonly property color danger: current.danger
    readonly property color info: current.info
    readonly property color idle: current.idle
    readonly property color okSoft: current.okSoft
    readonly property color warnSoft: current.warnSoft
    readonly property color dangerSoft: current.dangerSoft
    readonly property color infoSoft: current.infoSoft
    readonly property color glassTint: current.glassTint
    readonly property real glassBlur: current.glassBlur
    readonly property int glassBlurMax: current.glassBlurMax
    readonly property color glassEdge: current.glassEdge
    readonly property real glassShine: current.glassShine
    readonly property int radiusSmall: current.radiusSmall * uiScale
    readonly property int radiusMedium: current.radiusMedium * uiScale
    readonly property int radiusLarge: current.radiusLarge * uiScale
    readonly property color shadowColor: current.shadowColor
    readonly property color mapPatternA: current.mapPatternA
    readonly property color mapPatternB: current.mapPatternB
    readonly property int order: current.order
    readonly property bool dark: current.dark
    readonly property bool glass: current.glass
    readonly property bool shadow: current.shadow
    readonly property string titleKey: current.titleKey
    readonly property string fontFamily: current.fontFamily
    readonly property string monoFamily: current.monoFamily
}
