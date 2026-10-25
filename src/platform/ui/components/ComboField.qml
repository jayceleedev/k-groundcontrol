// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 선택 목록과 이름표를 함께 보여줘요.
 * 입력: model, textRole, valueRole, displayText, label, currentIndex와 activated를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.ComboBox {
    id: control
    property string label: ""
    implicitWidth: Theme.languageWidth
    implicitHeight: Theme.menuHeight + (label ? Theme.menuHeight : 0)
    topPadding: label ? Theme.menuHeight : Theme.inset
    leftPadding: Theme.inset
    rightPadding: Theme.menuHeight
    font.family: Theme.fontFamily
    font.pixelSize: Theme.fontSize
    contentItem: Text { text: control.displayText; color: control.enabled ? Theme.ink : Theme.inkDisabled; font: control.font; verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight }
    Text { text: control.label; color: Theme.inkMuted; font: control.font; x: Theme.inset; visible: text.length > 0 }
    indicator: IconGlyph { name: "chevron-down"; x: control.width - width - Theme.inset; y: control.height - Theme.menuHeight + (Theme.menuHeight - height) / 2 }
    background: Rectangle {
        y: control.label ? Theme.menuHeight : 0
        height: control.height - y
        color: control.down ? Theme.line : control.hovered ? Theme.surfaceAlt : Theme.surface
        radius: Theme.radiusSmall
        border.color: control.activeFocus ? Theme.focus : Theme.lineStrong
        border.width: control.activeFocus ? Theme.lineWidth * 2 : Theme.lineWidth
    }
    delegate: Basic.ItemDelegate {
        required property int index
        width: control.width
        text: control.textAt(index)
        font: control.font
        contentItem: Text { text: parent.text; color: Theme.ink; font: control.font }
        background: Rectangle { color: control.highlightedIndex === index ? Theme.infoSoft : Theme.surface }
    }
    popup: Basic.Popup {
        y: control.height
        width: control.width
        implicitHeight: Math.min(contentItem.implicitHeight + padding * 2, Theme.instrumentHeight)
        padding: Theme.lineWidth
        background: Rectangle { color: Theme.surface; border.color: Theme.lineStrong; radius: Theme.radiusSmall }
        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            Basic.ScrollIndicator.vertical: Basic.ScrollIndicator {}
        }
    }
}
