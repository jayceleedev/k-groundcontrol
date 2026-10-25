// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
Card {
    title: qsTr("차트")
    Flow {
        width: parent.width
        spacing: Theme.panelInset
        LineChart {
            objectName: "gallery_LineChart"
            seriesNames: [qsTr("목표"), qsTr("실제")]
            maxPoints: 100
            Component.onCompleted: {
                const target = [10, 40, 30, 70, 55, 75, 65]
                const actual = [5, 20, 50, 45, 60, 50, 70]
                for (let i = 0; i < target.length; ++i) { append(0, i, target[i]); append(1, i, actual[i]) }
            }
            xTo: 6
        }
        BarChart { objectName: "gallery_BarChart"; categories: [qsTr("앞"), qsTr("뒤"), qsTr("왼쪽"), qsTr("오른쪽")]; seriesNames: [qsTr("출력")]; values: [[60, 75, 45, 65]] }
        PieChart { objectName: "gallery_PieChart"; donut: true; values: [70, 30]; labels: [qsTr("사용"), qsTr("남음")]; text: qsTr("70%") }
        RingGauge { objectName: "gallery_RingGauge"; value: 82; unit: qsTr("% 배터리") }
        ArcGauge { objectName: "gallery_ArcGauge"; value: 38; unit: qsTr("A") }
        Column {
            spacing: Theme.panelInset
            Sparkline { objectName: "gallery_Sparkline"; values: [25, 22, 24, 15, 18, 10, 14, 8, 12] }
            Heatmap { objectName: "gallery_Heatmap"; values: [15, 30, 60, 90, 80, 100, 65, 70, 40, 20, 10, 5] }
        }
    }
}
