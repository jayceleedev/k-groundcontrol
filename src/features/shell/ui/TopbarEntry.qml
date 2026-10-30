// [0.0.3 T21] specs/features/0.0.3-windows.md
// [0.0.4 T33 T34 T35] specs/features/0.0.4-righttabs.md
/** 상단바 슬롯 한 항목을 모양에 맞춰 그려요. 자리 표시 단추는 눌려도 아직 하는 일이 없어요.
 * 입력: area(이 줄이 그리는 영역)와 슬롯 모델의 id·area·labelKey·icon·iconOnly·separatorBefore·shape.
 * 출력: 영역이 맞으면 앞 구분선과 활성 단추·드롭다운·글자 표시를, 아니면 아무것도 보이지 않아요.
 * 실패: 없음.
 */
import QtQuick
import Kgc.Ui
Row {
    id: entry
    required property var model
    property string area
    visible: model.area === area
    spacing: Theme.inset
    height: Theme.controlHeight
    Rectangle {
        visible: entry.model.separatorBefore
        width: Theme.lineWidth
        height: Theme.frameButtonSize
        color: Theme.lineStrong
        anchors.verticalCenter: parent.verticalCenter
    }
    Loader {
        anchors.verticalCenter: parent.verticalCenter
        sourceComponent: entry.model.shape === "display" ? displayEntry
            : entry.model.iconOnly ? iconEntry : buttonEntry
    }
    Component {
        id: displayEntry
        Text {
            objectName: "topbar_" + entry.model.id
            text: { translator.language; return qsTranslate("Bar", entry.model.labelKey) }
            color: Theme.ink
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            height: Theme.controlHeight
            verticalAlignment: Text.AlignVCenter
        }
    }
    Component {
        id: buttonEntry
        ActionButton {
            objectName: "topbar_" + entry.model.id
            kind: "secondary"
            text: { translator.language; return qsTranslate("Bar", entry.model.labelKey) }
            icon.source: entry.model.icon ? "qrc:/kgc/icons/" + entry.model.icon + ".svg" : ""
            trailingIcon: entry.model.shape === "dropdown" ? "chevron-down" : ""
        }
    }
    Component {
        id: iconEntry
        IconButton {
            objectName: "topbar_" + entry.model.id
            name: entry.model.icon
            hint: { translator.language; return qsTranslate("Bar", entry.model.labelKey) }
            trailingIcon: entry.model.shape === "dropdown" ? "chevron-down" : ""
        }
    }
}
