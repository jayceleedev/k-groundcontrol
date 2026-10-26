// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
// [0.0.4 T11] specs/features/0.0.4-righttabs.md
// [0.0.4 T21] specs/features/0.0.4-righttabs.md
import QtQuick
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
        // 계기는 토글로 기체 값 유무를, 슬라이더로 roll·pitch·heading을 바꿔 자세 그림을 확인해요.
        Column {
            spacing: Theme.inset
            AttitudeIndicator {
                objectName: "gallery_AttitudeIndicator"
                available: attitudeAvailable.checked
                roll: attitudeRoll.value
                pitch: attitudePitch.value
                heading: attitudeHeading.value
            }
            Toggle { id: attitudeAvailable; objectName: "gallery_AttitudeIndicator_available"; text: qsTr("기체 값 있음"); checked: true }
            SliderField { id: attitudeRoll; objectName: "gallery_AttitudeIndicator_roll"; from: -90; to: 90; stepSize: 1; unit: qsTr("° 롤") }
            SliderField { id: attitudePitch; objectName: "gallery_AttitudeIndicator_pitch"; from: -90; to: 90; stepSize: 1; unit: qsTr("° 피치") }
            SliderField { id: attitudeHeading; objectName: "gallery_AttitudeIndicator_heading"; from: 0; to: 360; stepSize: 1; unit: qsTr("° 헤딩") }
        }
        // PFD·HUD는 같은 토글·슬라이더 한 벌로 함께 움직여 두 모양을 나란히 비교해요.
        Column {
            spacing: Theme.inset
            Row {
                spacing: Theme.inset
                PrimaryFlightDisplay {
                    objectName: "gallery_PrimaryFlightDisplay"
                    available: instrumentAvailable.checked
                    roll: instrumentRoll.value
                    pitch: instrumentPitch.value
                    heading: instrumentHeading.value
                    speed: instrumentSpeed.value
                    altitude: instrumentAltitude.value
                }
                HeadUpDisplay {
                    objectName: "gallery_HeadUpDisplay"
                    available: instrumentAvailable.checked
                    roll: instrumentRoll.value
                    pitch: instrumentPitch.value
                    heading: instrumentHeading.value
                    speed: instrumentSpeed.value
                    altitude: instrumentAltitude.value
                }
            }
            Toggle { id: instrumentAvailable; objectName: "gallery_Instrument_available"; text: qsTr("기체 값 있음"); checked: true }
            SliderField { id: instrumentRoll; objectName: "gallery_Instrument_roll"; from: -90; to: 90; stepSize: 1; unit: qsTr("° 롤") }
            SliderField { id: instrumentPitch; objectName: "gallery_Instrument_pitch"; from: -90; to: 90; stepSize: 1; unit: qsTr("° 피치") }
            SliderField { id: instrumentHeading; objectName: "gallery_Instrument_heading"; from: 0; to: 360; stepSize: 1; unit: qsTr("° 헤딩") }
            SliderField { id: instrumentSpeed; objectName: "gallery_Instrument_speed"; from: 0; to: 40; stepSize: 0.1; unit: qsTr("m/s 속도") }
            SliderField { id: instrumentAltitude; objectName: "gallery_Instrument_altitude"; from: 0; to: 500; stepSize: 1; unit: qsTr("m 고도") }
        }
    }
}
