// [0.4.0 6.8] specs/features/0.4.0-sitl.md
/** 홈 위치 네 칸과 "해운대로 되돌리기" 단추를 보여요.
 * 입력: SitlController의 홈 값, badHomeFields, editable이에요.
 * 출력: 고친 값을 SitlController에 넘기고, 되돌리기는 resetHome을 불러요.
 * 실패: checkHome이 돌려준 칸마다 그 칸 아래 빨간 글(errorText(BadHome))을 보여요.
 * 흐름:
 * 1. SitlController.homeLatitude 등 — 고친 값을 넘겨요.
 * 2. SitlController.resetHome — 설정 기본값으로 채워요.
 */
import QtQuick
import Kgc.Ui
import Kgc.Sitl

Column {
    id: root
    spacing: Theme.inset
    enabled: SitlController.editable
    readonly property string badText: { const language = translator.language; return SitlController.badHomeText() }
    FontMetrics { id: metrics; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSize }
    // 범위 판정은 checkHome이 하므로 입력 칸은 숫자이기만 하면 받아들여요.
    component HomeField: Column {
        id: field
        required property string label
        required property string boxName
        required property int kind
        required property real source
        property int decimals: 0
        signal edited(real value)
        spacing: Theme.valueLabelGap
        Text { text: field.label; color: Theme.inkMuted; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSize }
        NumberBox {
            id: box
            objectName: field.boxName
            width: field.width
            from: -Number.MAX_VALUE
            to: Number.MAX_VALUE
            decimals: field.decimals
            value: field.source
            onValueModified: next => {
                field.edited(next)
                box.value = Qt.binding(() => field.source)
            }
        }
        Text {
            width: field.width
            visible: SitlController.badHomeFields.indexOf(field.kind) >= 0
            text: root.badText
            color: Theme.danger
            font.family: Theme.fontFamily
            font.pixelSize: Theme.captionSize
            wrapMode: Text.Wrap
        }
    }
    Text { text: qsTr("홈 위치"); color: Theme.inkMuted; font.family: Theme.fontFamily; font.pixelSize: Theme.fontSize; font.bold: true }
    Row {
        id: fields
        width: parent.width
        spacing: Theme.inset
        // 네 칸이 되돌리기 단추를 뺀 폭을 똑같이 나눠 가져요.
        readonly property real fieldWidth: (width - reset.width - spacing * 4) / 4
        HomeField {
            width: fields.fieldWidth
            label: qsTr("위도")
            boxName: "sitlHomeLatitude"
            kind: Sitl.Latitude
            source: SitlController.homeLatitude
            decimals: 7
            onEdited: value => SitlController.homeLatitude = value
        }
        HomeField {
            width: fields.fieldWidth
            label: qsTr("경도")
            boxName: "sitlHomeLongitude"
            kind: Sitl.Longitude
            source: SitlController.homeLongitude
            decimals: 7
            onEdited: value => SitlController.homeLongitude = value
        }
        HomeField {
            width: fields.fieldWidth
            label: qsTr("고도 (m)")
            boxName: "sitlHomeAltitude"
            kind: Sitl.Altitude
            source: SitlController.homeAltitude
            decimals: 1
            onEdited: value => SitlController.homeAltitude = value
        }
        HomeField {
            width: fields.fieldWidth
            label: qsTr("기수 (°)")
            boxName: "sitlHomeHeading"
            kind: Sitl.Heading
            source: SitlController.homeHeading
            onEdited: value => SitlController.homeHeading = Math.round(value)
        }
        ActionButton {
            id: reset
            objectName: "sitlHomeReset"
            y: Math.ceil(metrics.height) + Theme.valueLabelGap
            kind: "secondary"
            text: qsTr("해운대로 되돌리기")
            onClicked: SitlController.resetHome()
        }
    }
}
