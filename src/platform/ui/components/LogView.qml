// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 수준별 색을 가진 기록을 끝까지 따라가며 보여줘요.
 * 입력: model은 text, level(info, debug, warn, error) 목록이며 followTail을 받아요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.ScrollView {
    id: control
    property var model: []
    property bool followTail: true
    implicitWidth: Theme.languageWidth * 2
    implicitHeight: Theme.menuHeight * 4
    clip: true
    background: Rectangle { color: Theme.surfaceAlt; border.color: Theme.line; radius: Theme.radiusSmall }
    ListView {
        id: view
        model: control.model
        onCountChanged: if (control.followTail) Qt.callLater(positionViewAtEnd)
        delegate: Text {
            required property var modelData
            width: view.width
            text: modelData.text
            color: modelData.level === "error" ? Theme.danger : modelData.level === "warn" ? Theme.warn : modelData.level === "debug" ? Theme.inkMuted : Theme.ink
            font.family: Theme.monoFamily
            font.pixelSize: Theme.fontSize
            padding: Theme.menuGap
            wrapMode: Text.Wrap
        }
    }
}
