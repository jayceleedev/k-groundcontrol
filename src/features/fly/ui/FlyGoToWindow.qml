// [0.6.0 5.11.10 T19 T20 T21] specs/features/0.6.0-fly.md
/** 지도에서 고른 곳과 홈 기준 고도로 기체를 보내는 여기로 가기 창이에요(FlyGoToLayer가 좌표에 붙여 보여요).
 * 입력: 누른 좌표 point, FlyController의 canGoTo·고도 한계·기체 위치·goToHint·goToProblem·distanceText·positionText예요.
 * 출력: 제목, "위치 …", "기체에서 …", 고도(홈 기준) 칸, 도착 뒤 안내 글, 문제 글(있을 때만)과 [길게 눌러 가기][취소]예요.
 *       길게 눌러 보냈으면 sent, 취소를 누르면 cancelled를 내요. 창 안의 누르기·끌기·휠은 지도로 내려가지 않아요.
 * 실패: 거리·고도가 한계 밖이거나 고도 칸 값이 범위 밖이면 확인 단추가 흐려요. 보내기 검사가 실패하면 토스트가 나고 창은 그대로예요.
 * 흐름:
 * 1. prepare — 화면 점과 지도 폭·창 크기로 뒤집기를 정하고, 새로 열 때만 고도 칸(기체 지금 고도)과 기체 sysid를 정해요.
 * 2. flyGoToConfirm — 누르는 순간 고도 칸이 확정되고, 다 누르면 FlyController.goTo(창을 연 때의 기체)가 FlyService::goTo를 불러요.
 * 3. 보냈으면 sent로 창이 닫혀요.
 */
import QtQuick
import QtQuick.Controls
import Kgc.Ui
import Kgc.Fly

Item {
    id: goToWindow
    objectName: "flyGoToWindow"
    width: Theme.flyWindowWidth
    height: content.implicitHeight + Theme.inset * 2
    property var point
    property int sysid: -1
    property bool flipX: false
    property bool flipY: false
    signal cancelled()
    signal sent()
    // 뒤집기는 창을 열거나 옮길 때만 정해요. 그 뒤 창이 길어지면 고정된 쪽(뒤집지 않으면 아래, 뒤집으면 위)에서 늘어나요.
    function prepare(screenPoint, mapWidth, fresh) {
        flipX = screenPoint.x + Theme.inset + width > mapWidth
        flipY = screenPoint.y - Theme.inset - height < 0
        if (fresh) {
            // NumberBox의 글은 value가 바뀔 때만 value를 따라가요. 같은 값을 다시 넣으면 적다 만(확정하지 않은) 글이 남으므로
            // 다른 값(NaN)을 먼저 넣은 뒤 진짜 값을 넣어 글이 늘 기본 고도를 보이게 해요.
            altitudeBox.value = Number.NaN
            altitudeBox.value = FlyController.defaultGoToAltitude()
            sysid = FlyController.sysid
        }
    }
    // goToProblem은 함수라 QML이 안에서 읽는 값을 모르므로, 결과가 기대는 기체 위치·한계를 읽어 바뀌면 다시 계산되게 해요.
    readonly property string problem: {
        const language = translator.language
        const position = FlyController.vehicleCoordinate
        const limit = FlyController.guidedMaxDistanceM
        const from = FlyController.guidedAltitudeFrom
        const to = FlyController.guidedAltitudeTo
        return FlyController.goToProblem(point.latitude, point.longitude, altitudeBox.value)
    }
    // 창 위에서 시작한 누르기·끌기·휠이 아래 지도로 내려가지 않게 받아요.
    MouseArea {
        objectName: "flyGoToInputBlock"
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        onWheel: (wheel) => wheel.accepted = true
    }
    // 내용의 TapHandler가 먼저 받아 MouseArea까지 오지 않은 누름도 독점으로 잡아 지도 끌기가 가져가지 못하게 해요.
    TapHandler { objectName: "flyGoToTapBlock"; gesturePolicy: TapHandler.DragWithinBounds }
    Rectangle {
        anchors.fill: parent
        color: Theme.surface
        border.color: Theme.lineStrong
        radius: Theme.radiusSmall
    }
    Column {
        id: content
        x: Theme.inset
        y: Theme.inset
        width: parent.width - Theme.inset * 2
        spacing: Theme.inset
        Text {
            objectName: "flyGoToTitle"
            text: qsTr("여기로 가기")
            font.bold: true
            font.family: Theme.fontFamily
            font.pixelSize: Theme.titleSize
            color: Theme.ink
        }
        Text {
            objectName: "flyGoToPosition"
            text: qsTr("위치 %1").arg(FlyController.positionText(goToWindow.point.latitude, goToWindow.point.longitude))
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            color: Theme.ink
        }
        Text {
            objectName: "flyGoToDistance"
            text: {
                const language = translator.language
                const position = FlyController.vehicleCoordinate
                return qsTr("기체에서 %1").arg(FlyController.distanceText(goToWindow.point.latitude, goToWindow.point.longitude))
            }
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            color: Theme.ink
        }
        Label { text: qsTr("고도(홈 기준)"); color: Theme.inkMuted }
        NumberBox {
            id: altitudeBox
            objectName: "flyGoToAltitude"
            from: FlyController.guidedAltitudeFrom
            to: FlyController.guidedAltitudeTo
            decimals: 1
            unit: qsTr("m")
            width: parent.width
        }
        Text {
            objectName: "flyGoToHint"
            text: { const language = translator.language; return FlyController.goToHint }
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            color: Theme.inkMuted
            wrapMode: Text.Wrap
            width: parent.width
        }
        Text {
            objectName: "flyGoToProblem"
            text: goToWindow.problem
            visible: text !== ""
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            color: Theme.danger
            wrapMode: Text.Wrap
            width: parent.width
        }
        Row {
            spacing: Theme.inset
            FlyHoldButton {
                objectName: "flyGoToConfirm"
                text: qsTr("길게 눌러 가기")
                enabled: FlyController.canGoTo && goToWindow.problem === "" && altitudeBox.acceptableInput
                onConfirmed: if (FlyController.goTo(goToWindow.sysid, goToWindow.point.latitude, goToWindow.point.longitude, altitudeBox.value)) goToWindow.sent()
            }
            ActionButton {
                objectName: "flyGoToCancel"
                kind: "secondary"
                text: qsTr("취소")
                onClicked: goToWindow.cancelled()
            }
        }
    }
}
