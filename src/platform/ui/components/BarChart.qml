// [0.0.2 T12 T22] specs/features/0.0.2-theme.md
/** 범주별 값을 묶은 막대로 비교해요.
 * 입력: categories, seriesNames, values(줄별 값 목록), horizontal, from, to, decimals(기본 0)를 받아요.
 * 출력: values의 행마다 막대 묶음 하나와 decimals 자리의 값 글자를 표시해요.
 * 실패: 없는 계열 이름은 빈 글자로 표시해요. 숫자와 범주 길이의 유효성은 호출자가 보장해요.
 */
import QtQuick
import Kgc.Ui
import QtGraphs

Item {
    id: chart
    property list<string> categories: []
    property list<string> seriesNames: []
    property var values: []
    property int decimals: 0
    property bool horizontal: false
    property real from: 0
    property real to: 100
    implicitWidth: Theme.chartWidth
    implicitHeight: Theme.chartHeight
    // 생성 중 들어오는 값과 이름 변경을 모아 계열이 준비된 뒤 한 번만 채워요.
    Component.onCompleted: Qt.callLater(rebuild)
    function rebuild() {
        series.clear()
        for (let i = 0; i < values.length; ++i) {
            const set = setFactory.createObject(chart, {label: seriesNames[i] || "", values: values[i], colorIndex: i})
            series.append(set)
        }
    }
    onValuesChanged: Qt.callLater(rebuild)
    onSeriesNamesChanged: Qt.callLater(rebuild)
    Component {
        id: setFactory
        BarSet { property int colorIndex: 0; color: Theme.seriesColors[colorIndex % Theme.seriesColors.length]; borderColor: color; labelColor: Theme.inkMuted }
    }
    GraphsView {
        anchors.fill: parent
        anchors.bottomMargin: Theme.chartLegendHeight
        marginLeft: Theme.inset; marginRight: Theme.inset; marginTop: Theme.inset; marginBottom: Theme.inset
        orientation: chart.horizontal ? Qt.Horizontal : Qt.Vertical
        axisX: BarCategoryAxis { categories: chart.categories }
        axisY: ValueAxis { min: chart.from; max: chart.to; labelDecimals: 0 }
        theme: ChartTheme {}
        BarSeries { id: series; labelsVisible: true; labelsPrecision: chart.decimals }
    }
    Row {
        anchors.bottom: parent.bottom
        spacing: Theme.panelInset
        Repeater {
            model: chart.seriesNames
            Text { required property string modelData; text: modelData; color: Theme.ink; font.pixelSize: Theme.fontSize }
        }
    }
}
