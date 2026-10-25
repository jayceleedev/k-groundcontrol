// [0.0.1 T1] specs/features/0.0.1-shell.md
// [0.0.2 T8 T12 T13 T22] specs/features/0.0.2-theme.md
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
    // 좌 메뉴 묶음 사이 구분선이 차지하는 세로 공간이에요(선은 그 가운데).
    readonly property int menuSeparatorSpace: Math.round(17 * uiScale)
    readonly property int menuHeight: Math.round(36 * uiScale)
    readonly property int iconSize: Math.round(20 * uiScale)
    readonly property int appIconSize: Math.round(40 * uiScale)
    readonly property int appIconPadding: 0
    readonly property int rightTabHeight: Math.round(32 * uiScale)
    readonly property int panelHeaderHeight: Math.round(48 * uiScale)
    readonly property real lineWidth: 1 * uiScale
    readonly property int fontSize: Math.round(13 * uiScale)
    readonly property int titleSize: Math.round(14 * uiScale)
    readonly property int dialogWidth: Math.round(720 * uiScale)
    readonly property int dialogHeight: Math.round(560 * uiScale)
    readonly property int languageWidth: Math.round(240 * uiScale)
    // 단추·선택·입력 부품의 기본 높이를 맞춰요.
    readonly property int controlHeight: Math.round(36 * uiScale)
    // 다른 부품 안에 들어가는 테두리 없는 아이콘 단추(닫기·지우기)의 누르는 영역과 아이콘 크기예요.
    readonly property int flatIconButtonSize: Math.round(24 * uiScale)
    readonly property int flatIconSize: Math.round(16 * uiScale)
    // 텍스트·숫자·목록 입력란의 기본 너비예요.
    readonly property int inputWidth: Math.round(240 * uiScale)
    // 입력란의 이름표와 오류 글에 한 줄을 확보해요.
    readonly property int inputLabelHeight: Math.round(36 * uiScale)
    // 목록 선택 창이 지나치게 길어지지 않게 해요.
    readonly property int comboPopupHeight: Math.round(240 * uiScale)
    // 선택 단추 사이의 간격을 정해요.
    readonly property int choiceGap: Math.round(3 * uiScale)
    // 길게 누르는 진행선을 눈에 띄게 해요.
    readonly property int holdProgressHeight: Math.round(3 * uiScale)
    // 회전 표시의 방향 표식 두께예요.
    readonly property int spinnerMarkHeight: Math.round(3 * uiScale)
    // 토글 손잡이가 테두리에 닿지 않게 해요.
    readonly property int switchInset: Math.round(3 * uiScale)
    // 값 타일에 필요한 최소 너비예요.
    readonly property int valueTileWidth: Math.round(108 * uiScale)
    // 큰 값과 그 아래 단위 사이를 띄워요.
    readonly property int valueLabelGap: Math.round(3 * uiScale)
    // 이름과 값을 나란히 읽는 줄의 기본 너비예요.
    readonly property int keyValueWidth: Math.round(240 * uiScale)
    // 양방향 값 막대의 기본 너비예요.
    readonly property int levelBarWidth: Math.round(240 * uiScale)
    // 막대 밖으로 드러나는 눈금 길이예요.
    readonly property int levelTickLength: Math.round(3 * uiScale)
    // 진행 막대의 기본 너비예요.
    readonly property int progressWidth: Math.round(240 * uiScale)
    // 축이 있는 차트의 기본 너비예요.
    readonly property int chartWidth: Math.round(480 * uiScale)
    // 축이 있는 차트의 기본 높이예요.
    readonly property int chartHeight: Math.round(240 * uiScale)
    // 차트 아래 범례 자리를 확보해요.
    readonly property int chartLegendHeight: Math.round(36 * uiScale)
    // 원형 차트의 기본 지름이에요.
    readonly property int pieDiameter: Math.round(240 * uiScale)
    // 작은 추세선의 기본 너비예요.
    readonly property int sparklineWidth: Math.round(240 * uiScale)
    // 작은 추세선의 기본 높이예요.
    readonly property int sparklineHeight: Math.round(36 * uiScale)
    // 고리 계기의 기본 지름이에요.
    readonly property int ringDiameter: Math.round(144 * uiScale)
    // 반원 계기의 기본 너비예요.
    readonly property int arcGaugeWidth: Math.round(240 * uiScale)
    // 반원 계기와 값 글자에 필요한 높이예요.
    readonly property int arcGaugeHeight: Math.round(156 * uiScale)
    // 칸 색 지도의 기본 너비예요.
    readonly property int heatmapWidth: Math.round(240 * uiScale)
    // 표의 기본 너비예요.
    readonly property int dataTableWidth: Math.round(480 * uiScale)
    // 표에서 한 줄을 읽기 좋은 높이예요.
    readonly property int dataRowHeight: Math.round(36 * uiScale)
    // 기록 목록의 기본 너비예요.
    readonly property int logViewWidth: Math.round(480 * uiScale)
    // 기록 목록의 기본 높이예요.
    readonly property int logViewHeight: Math.round(144 * uiScale)
    // 기록 글자와 줄 경계 사이를 띄워요.
    readonly property int logLinePadding: Math.round(3 * uiScale)
    // 확인 대화 상자의 기본 너비예요.
    readonly property int confirmDialogWidth: Math.round(480 * uiScale)
    // ThemeSwitcher만 current를 바꿔요. 부품은 Theme의 토큰만 읽어요.
    property ThemeBase current: Light {}
    readonly property color series1: current.series1
    readonly property color series2: current.series2
    readonly property color series3: current.series3
    readonly property color series4: current.series4
    readonly property color series5: current.series5
    readonly property color series6: current.series6
    readonly property list<color> seriesColors: [series1, series2, series3, series4, series5, series6]
    readonly property color chartGrid: current.chartGrid
    readonly property color chartAxis: current.chartAxis
    readonly property color dangerInk: current.dangerInk
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
    readonly property color accentInk: current.accentInk
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
    readonly property int radiusSmall: Math.round(current.radiusSmall * uiScale)
    readonly property int radiusMedium: Math.round(current.radiusMedium * uiScale)
    readonly property int radiusLarge: Math.round(current.radiusLarge * uiScale)
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
