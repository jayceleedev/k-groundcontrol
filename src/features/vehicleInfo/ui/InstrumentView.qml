// [0.0.4 T3] specs/features/0.0.4-righttabs.md
// 우 사이드바 계기 칸: 기체 값이 아직 없어(0.1.x부터) "기체 없음" 계기 하나를 칸 가운데 둬요.
import QtQuick
import Kgc.Ui
Item {
    AttitudeIndicator { anchors.centerIn: parent; available: false }
}
