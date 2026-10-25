// [0.0.2 T12 T22] specs/features/0.0.2-theme.md
/** 시간에 따른 여러 값을 확대하고 이동하며 비교해요.
 * 입력: seriesNames, maxPoints, xFrom/xTo, yFrom/yTo와 append(seriesIndex, x, y), clear(), seriesAt(index)를 사용해요.
 * 출력: lines와 seriesAt(index)로 계열을 제공하고 점·축·범례를 그려요.
 * 실패: 없는 계열이나 유한하지 않은 점은 append에서 무시해요. 잘못된 위치의 seriesAt은 null이에요.
 */
import QtQuick
import Kgc.Ui
import QtGraphs

Item {
    id: chart
    property list<string> seriesNames: []
    property int maxPoints: 0
    property real xFrom: 0
    property real xTo: 10
    property real yFrom: 0
    property real yTo: 100
    property list<QtObject> lines: []
    implicitWidth: Theme.chartWidth
    implicitHeight: Theme.chartHeight
    function seriesAt(index: int): QtObject { return index >= 0 && index < lines.length ? lines[index] : null }
    function append(seriesIndex: int, x: real, y: real) {
        const series = seriesAt(seriesIndex)
        if (!series || !Number.isFinite(x) || !Number.isFinite(y)) return
        series.append(x, y)
        if (maxPoints > 0 && series.count > maxPoints) series.removeMultiple(0, series.count - maxPoints)
    }
    function clear() { for (const series of lines) series.clear() }
    function rebuild() {
        for (const series of lines) { graph.removeSeries(series); series.destroy() }
        let next = []
        for (let i = 0; i < seriesNames.length; ++i) {
            const series = lineFactory.createObject(chart, {name: seriesNames[i], colorIndex: i})
            graph.addSeries(series)
            next.push(series)
        }
        lines = next
    }
    onSeriesNamesChanged: rebuild()
    onMaxPointsChanged: {
        if (maxPoints > 0) for (const series of lines)
            if (series.count > maxPoints) series.removeMultiple(0, series.count - maxPoints)
    }
    Component {
        id: lineFactory
        LineSeries {
            property int colorIndex: 0
            color: Theme.seriesColors[colorIndex % Theme.seriesColors.length]
            width: Theme.lineWidth * 2
        }
    }
    QtObject {
        id: ticks
        // 보이는 범위와 글자 자리를 함께 보고 간격을 정해 축소해도 눈금이 겹치지 않게 해요.
        function interval(span: real, pixels: real, spacing: real): real {
            if (span <= 0) return 1
            const step = span / Math.max(1, Math.floor(pixels / spacing))
            const magnitude = Math.pow(10, Math.floor(Math.log10(step)))
            return Math.ceil(step / magnitude) * magnitude
        }
    }
    FontMetrics { id: labelMetrics; font.pixelSize: Theme.fontSize }
    GraphsView {
        id: graph
        anchors.fill: parent
        anchors.bottomMargin: Theme.chartLegendHeight
        marginLeft: Theme.inset; marginRight: Theme.inset; marginTop: Theme.inset; marginBottom: Theme.inset
        dynamicLabelMargins: true
        axisX: ValueAxis {
            min: chart.xFrom; max: chart.xTo
            tickInterval: ticks.interval(visualMax - visualMin, graph.plotArea.width,
                Math.max(labelMetrics.advanceWidth(visualMin.toPrecision(6)),
                         labelMetrics.advanceWidth(visualMax.toPrecision(6))) + Theme.inset * 2)
            labelDecimals: Math.max(0, -Math.floor(Math.log10(tickInterval)))
        }
        axisY: ValueAxis {
            min: chart.yFrom; max: chart.yTo
            tickInterval: ticks.interval(visualMax - visualMin, graph.plotArea.height,
                                         labelMetrics.height + Theme.inset)
            labelDecimals: Math.max(0, -Math.floor(Math.log10(tickInterval)))
        }
        panStyle: GraphsView.PanStyle.Drag
        zoomStyle: GraphsView.ZoomStyle.Center
        theme: ChartTheme {}
    }
    Row {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: Theme.panelInset
        Repeater {
            model: chart.seriesNames
            Row {
                required property int index
                required property string modelData
                spacing: Theme.inset
                Rectangle { width: Theme.iconSize; height: Theme.lineWidth * 2; anchors.verticalCenter: parent.verticalCenter; color: Theme.seriesColors[parent.index % Theme.seriesColors.length] }
                Text { text: parent.modelData; color: Theme.ink; font.pixelSize: Theme.fontSize }
            }
        }
    }
}
