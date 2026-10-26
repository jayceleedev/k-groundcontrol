// [0.0.2 T12 T14] specs/features/0.0.2-theme.md
// [0.0.4 T6a] specs/features/0.0.4-righttabs.md
/** 선택 목록과 이름표를 함께 보여줘요.
 * 입력: model, textRole, valueRole, displayText, label, currentIndex와 activated를 사용해요.
 * 출력: 고른 값을 displayText로 보여 주고 currentIndex와 activated를 내요.
 *       enabled가 거짓이면 ActionButton처럼 흐리게(바탕 surfaceAlt, 글자 inkDisabled, 테두리 line) 그려요.
 * 실패: 목록 밖 currentIndex에는 선택된 항목이 없고, 빈 목록에는 고를 항목이 없어요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.ComboBox {
    id: control
    property string label: ""
    implicitWidth: Theme.inputWidth
    implicitHeight: Theme.controlHeight + (label ? Theme.inputLabelHeight : 0)
    topPadding: label ? Theme.inputLabelHeight : Theme.inset
    leftPadding: Theme.inset
    rightPadding: Theme.controlHeight
    font.family: Theme.fontFamily
    font.pixelSize: Theme.fontSize
    contentItem: Text { text: control.displayText; color: control.enabled ? Theme.ink : Theme.inkDisabled; font: control.font; verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight }
    Text { text: control.label; color: control.enabled ? Theme.inkMuted : Theme.inkDisabled; font: control.font; x: Theme.inset; visible: text.length > 0 }
    indicator: IconGlyph { name: "chevron-down"; color: control.enabled ? Theme.ink : Theme.inkDisabled; x: control.width - width - Theme.inset; y: control.height - Theme.controlHeight + (Theme.controlHeight - height) / 2 }
    background: Rectangle {
        y: control.label ? Theme.inputLabelHeight : 0
        height: control.height - y
        color: !control.enabled ? Theme.surfaceAlt : control.down ? Theme.line : control.hovered ? Theme.surfaceAlt : Theme.surface
        radius: Theme.radiusSmall
        border.color: !control.enabled ? Theme.line : control.activeFocus ? Theme.focus : Theme.lineStrong
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
        implicitHeight: Math.min(contentItem.implicitHeight + padding * 2, Theme.comboPopupHeight)
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
