// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 숫자의 단위와 허용 범위를 보여줘요.
 * 입력: value, from, to, stepSize, decimals, unit, valueModified(value)를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

FocusScope {
    id: control
    property real value: 0
    property real from: 0
    property real to: 100
    property real stepSize: 1
    property int decimals: 0
    property string unit: ""
    readonly property bool acceptableInput: Number.isFinite(Number(field.text)) && Number(field.text) >= from && Number(field.text) <= to
    signal valueModified(real value)
    implicitWidth: Theme.languageWidth
    implicitHeight: field.implicitHeight
    function commit(next: real) {
        if (Number.isFinite(next) && next >= from && next <= to) { value = next; valueModified(next) }
    }
    TextBox {
        id: field
        anchors.left: parent.left; anchors.right: up.left
        text: control.value.toFixed(control.decimals)
        errorText: control.acceptableInput ? "" : qsTr("%1~%2 사이 값을 입력해 주세요").arg(control.from).arg(control.to)
        rightPadding: unitLabel.implicitWidth + Theme.inset * 2
        onEditingFinished: control.commit(Number(text))
        Keys.onUpPressed: control.commit(Math.min(control.to, control.value + control.stepSize))
        Keys.onDownPressed: control.commit(Math.max(control.from, control.value - control.stepSize))
        Text { id: unitLabel; text: control.unit; color: Theme.inkMuted; anchors.right: parent.right; anchors.rightMargin: Theme.inset; y: Theme.inset; font: field.font }
    }
    IconButton { id: up; anchors.right: down.left; name: "chevron-up"; hint: qsTr("늘리기"); onClicked: control.commit(Math.min(control.to, control.value + control.stepSize)) }
    IconButton { id: down; anchors.right: parent.right; name: "chevron-down"; hint: qsTr("줄이기"); onClicked: control.commit(Math.max(control.from, control.value - control.stepSize)) }
}
