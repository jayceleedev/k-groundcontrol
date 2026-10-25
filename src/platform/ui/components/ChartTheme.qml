// [0.0.2 T12 T22] specs/features/0.0.2-theme.md
// [0.0.2 T26] specs/features/0.0.2-theme.md
/** 세 차트의 선과 글자 색을 같은 테마 토큰에 묶어요.
 * 입력: Theme의 차트 색과 글꼴 크기를 읽어요.
 * 출력: 격자·축·계열·글자 색이 연결된 GraphsTheme을 제공해요.
 * 실패: 토큰 누락은 QML 생성 오류로 드러나요.
 */
import QtGraphs
import Kgc.Ui

GraphsTheme {
    // 내장 색 모드를 바꾸면 축 색이 초기화되므로 사용자 테마의 토큰만 갱신해요.
    theme: GraphsTheme.Theme.UserDefined
    colorScheme: GraphsTheme.ColorScheme.Light
    backgroundVisible: false
    plotAreaBackgroundVisible: false
    seriesColors: Theme.seriesColors
    grid.mainWidth: Theme.lineWidth
    grid.subWidth: Theme.lineWidth
    axisX.mainWidth: Theme.lineWidth
    axisX.subWidth: Theme.lineWidth
    axisY.mainWidth: Theme.lineWidth
    axisY.subWidth: Theme.lineWidth
    grid.mainColor: Theme.chartGrid
    grid.subColor: Theme.chartGrid
    axisX.mainColor: Theme.chartAxis
    axisY.mainColor: Theme.chartAxis
    axisX.subColor: Theme.chartAxis
    axisY.subColor: Theme.chartAxis
    axisX.labelTextColor: Theme.inkMuted
    axisY.labelTextColor: Theme.inkMuted
    labelTextColor: Theme.inkMuted
    labelFont.pixelSize: Theme.fontSize
    axisXLabelFont.pixelSize: Theme.fontSize
    axisYLabelFont.pixelSize: Theme.fontSize
}
