// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 시간에 따른 여러 값을 확대하고 이동하며 비교해요.
 * 입력: seriesNames, maxPoints, xFrom/xTo, yFrom/yTo와 append(seriesIndex, x, y), clear(), seriesAt(index)를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
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
    implicitWidth: Theme.languageWidth * 2
    implicitHeight: Theme.instrumentHeight
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
    GraphsView {
        id: graph
        anchors.fill: parent
        anchors.bottomMargin: Theme.menuHeight
        marginLeft: Theme.inset; marginRight: Theme.inset; marginTop: Theme.inset; marginBottom: Theme.inset
        axisX: ValueAxis { min: chart.xFrom; max: chart.xTo; labelDecimals: 0 }
        axisY: ValueAxis { min: chart.yFrom; max: chart.yTo; labelDecimals: 0 }
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
