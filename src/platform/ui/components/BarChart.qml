// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 범주별 값을 묶은 막대로 비교해요.
 * 입력: categories, seriesNames, values(줄별 값 목록), horizontal, from, to를 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
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
    implicitWidth: Theme.languageWidth * 2
    implicitHeight: Theme.instrumentHeight
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
        anchors.bottomMargin: Theme.menuHeight
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
