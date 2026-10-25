// [0.0.2 T12 T22] specs/features/0.0.2-theme.md
/** 전체에서 차지하는 비율을 원과 고리로 보여줘요.
 * 입력: values, labels, donut, text를 받아요.
 * 출력: 값마다 조각 하나를 그리고 donut이면 가운데를 비워 text를 표시해요.
 * 실패: 음수 조각 값은 0으로 바꾸고 없는 labels는 빈 글자로 표시해요.
 */
import QtQuick
import Kgc.Ui
import QtGraphs

Item {
    id: chart
    property var values: []
    property list<string> labels: []
    property bool donut: false
    property string text
    implicitWidth: Theme.pieDiameter
    implicitHeight: Theme.pieDiameter
    // 생성 중 들어오는 값과 이름 변경을 모아 계열이 준비된 뒤 한 번만 채워요.
    Component.onCompleted: Qt.callLater(rebuild)
    function rebuild() {
        series.clear()
        for (let i = 0; i < values.length; ++i) {
            const slice = series.append(labels[i] || "", Math.max(0, values[i]))
            slice.color = Qt.binding(() => Theme.seriesColors[i % Theme.seriesColors.length])
            slice.borderColor = Qt.binding(() => slice.color)
        }
    }
    onValuesChanged: Qt.callLater(rebuild)
    onLabelsChanged: Qt.callLater(rebuild)
    GraphsView {
        anchors.fill: parent
        marginLeft: 0; marginRight: 0; marginTop: 0; marginBottom: 0
        theme: ChartTheme {}
        PieSeries { id: series; holeSize: chart.donut ? 0.65 : 0 }
    }
    Text { anchors.centerIn: parent; visible: chart.donut; text: chart.text; color: Theme.ink; font.pixelSize: Theme.titleSize }
}
