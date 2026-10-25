// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 전체에서 차지하는 비율을 원과 고리로 보여줘요.
 * 입력: values, labels, donut, text를 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import QtGraphs

Item {
    id: chart
    property var values: []
    property list<string> labels: []
    property bool donut: false
    property string text
    implicitWidth: Theme.instrumentHeight
    implicitHeight: Theme.instrumentHeight
    function rebuild() {
        series.clear()
        for (let i = 0; i < values.length; ++i) {
            const slice = series.append(labels[i] || "", Math.max(0, values[i]))
            slice.color = Qt.binding(() => [Theme.series1, Theme.series2, Theme.series3, Theme.series4, Theme.series5, Theme.series6][i % 6])
            slice.borderColor = Qt.binding(() => slice.color)
        }
    }
    onValuesChanged: rebuild()
    onLabelsChanged: rebuild()
    GraphsView {
        anchors.fill: parent
        marginLeft: 0; marginRight: 0; marginTop: 0; marginBottom: 0
        theme: GraphsTheme {
            colorScheme: Theme.dark ? GraphsTheme.ColorScheme.Dark : GraphsTheme.ColorScheme.Light
            backgroundVisible: false
            plotAreaBackgroundVisible: false
            seriesColors: [Theme.series1, Theme.series2, Theme.series3, Theme.series4, Theme.series5, Theme.series6]
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
        PieSeries { id: series; holeSize: chart.donut ? 0.65 : 0 }
    }
    Text { anchors.centerIn: parent; visible: chart.donut; text: chart.text; color: Theme.ink; font.pixelSize: Theme.titleSize }
}
