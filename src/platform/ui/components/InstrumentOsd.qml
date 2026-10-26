// [0.0.4 T20 T23 T23a] specs/features/0.0.4-righttabs.md
/** PFD·HUD가 함께 쓰는 OSD 글자 여섯 개(링크·모드 · 시동·상승률·홈 거리·배터리·GPS)를 그림 칸 가장자리에 얹어요.
 * 입력: available, flightMode(문자열), armed(참·거짓, 모르면 undefined), batteryVoltage(V)·batteryPercent(%), gpsFix(문자열)·satellites(개),
 *       homeDistance(m), climbRate(m/s), linkQuality(%)를 받아요. inkColor·outlined로 글자색과 그림자 테두리를,
 *       bottomInset으로 맨 아래에 비워 둘 높이(PFD의 방위 띠)를 정해요.
 * 출력: 위 왼쪽 링크, 위 오른쪽 모드 · 시동, 아래쪽 세 줄(상승률, 홈 거리, 왼쪽 배터리·오른쪽 GPS)을 그려요.
 *       topReserve·bottomReserve는 띠를 놓을 때 피해야 할 위·아래 높이로, 글자 상자의 실제 높이를 더해 구해요.
 * 실패: 숫자가 아니거나 빈 값이면 그 자리만 "—"로 쓰고, available이 거짓이면 모든 자리가 "—"예요.
 *       모드·GPS fix 글자가 자기 자리 폭에 안 들어가면 그 글자 끝만 …로 줄여요.
 */
import QtQuick
import Kgc.Ui

Item {
    id: osd
    property bool available: false
    property string flightMode: ""
    property var armed: undefined
    property real batteryVoltage: NaN
    property real batteryPercent: NaN
    property string gpsFix: ""
    property real satellites: NaN
    property real homeDistance: NaN
    property real climbRate: NaN
    property real linkQuality: NaN
    property color inkColor: Theme.osdInk
    property bool outlined: true
    property real bottomInset: 0
    // 한글·"—"·"↕"는 다른 글꼴로 그려져 FontMetrics 한 줄보다 1~2 px 커질 수 있어요.
    // 그래서 줄 간격은 글자 상자(Text)의 실제 높이로 쌓아요.
    readonly property real bottomRowHeight: Math.max(battery.height, gps.height)
    readonly property real topReserve: Math.max(link.height, modeArm.height)
    readonly property real bottomReserve: bottomInset + bottomRowHeight + home.height + climb.height
    readonly property real sidePadding: Theme.inset / 2
    readonly property string dash: "—"
    readonly property string armText: !available ? dash : armed === true ? "ARMED" : armed === false ? "DISARMED" : dash
    // 단위·기호만 있는 형식(ARMED·V·m/s는 영어에서도 그대로)이라 번역하지 않아요.
    readonly property string modeTailText: " · " + armText
    readonly property string climbText: "↕ " + fixed1(climbRate) + " m/s"
    readonly property string batteryText: fixed1(batteryVoltage) + " V · " + whole(batteryPercent) + " %"

    // 값이 있을 때만 숫자를 쓰고, 소수 한 자리는 toFixed(1), 정수는 Math.round로 맞춰요(음수 부호는 ASCII -).
    function has(value) { return available && typeof value === "number" && isFinite(value) }
    function fixed1(value) { return has(value) ? value.toFixed(1) : dash }
    function whole(value) { return has(value) ? String(Math.round(value)) : dash }
    function word(value) { return available && typeof value === "string" && value.length > 0 ? value : dash }

    FontMetrics { id: metrics; font.family: Theme.fontFamily; font.pixelSize: Theme.captionSize }
    // 모드는 " · ARMED" 꼬리를 뺀 남은 폭에서, GPS fix는 형식의 나머지 글자를 뺀 남은 폭에서만 줄여요.
    TextMetrics {
        id: modeTail
        font: metrics.font
        text: osd.modeTailText
    }
    TextMetrics {
        id: modeFit
        font: metrics.font
        text: osd.word(osd.flightMode)
        elide: Text.ElideRight
        elideWidth: Math.max(0, osd.width - osd.sidePadding * 3 - link.implicitWidth - modeTail.advanceWidth - Theme.lineWidth)
    }
    TextMetrics {
        id: gpsRest
        font: metrics.font
        text: qsTr("GPS %1 · 위성 %2").arg("").arg(osd.whole(osd.satellites))
    }
    TextMetrics {
        id: gpsFit
        font: metrics.font
        text: osd.word(osd.gpsFix)
        elide: Text.ElideRight
        elideWidth: Math.max(0, osd.width - osd.sidePadding * 3 - battery.implicitWidth - gpsRest.advanceWidth - Theme.lineWidth)
    }

    component OsdText: Text {
        font: metrics.font
        color: osd.inkColor
        style: osd.outlined ? Text.Outline : Text.Normal
        styleColor: Theme.osdShadow
    }
    OsdText {
        id: link
        objectName: "osdLink"
        x: osd.sidePadding
        text: qsTr("링크 %1%").arg(osd.whole(osd.linkQuality))
    }
    OsdText {
        id: modeArm
        objectName: "osdModeArm"
        anchors.right: parent.right
        anchors.rightMargin: osd.sidePadding
        text: modeFit.elidedText + modeTail.text
    }
    OsdText {
        id: climb
        objectName: "osdClimb"
        anchors.right: parent.right
        anchors.rightMargin: osd.sidePadding
        y: home.y - height
        text: osd.climbText
    }
    OsdText {
        id: home
        objectName: "osdHome"
        anchors.right: parent.right
        anchors.rightMargin: osd.sidePadding
        y: osd.height - osd.bottomInset - osd.bottomRowHeight - height
        text: qsTr("홈 %1 m").arg(osd.whole(osd.homeDistance))
    }
    OsdText {
        id: battery
        objectName: "osdBattery"
        x: osd.sidePadding
        y: osd.height - osd.bottomInset - height
        text: osd.batteryText
    }
    OsdText {
        id: gps
        objectName: "osdGps"
        anchors.right: parent.right
        anchors.rightMargin: osd.sidePadding
        y: osd.height - osd.bottomInset - height
        text: qsTr("GPS %1 · 위성 %2").arg(gpsFit.elidedText).arg(osd.whole(osd.satellites))
    }
}
