// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 수준별 색을 가진 기록을 끝까지 따라가며 보여줘요.
 * 입력: model은 text, level(info, debug, warn, error) 목록이며 followTail을 받아요.
 * 출력: 수준별 색으로 기록을 표시하고 followTail이면 새 행이 생길 때 끝으로 이동해요.
 * 실패: 알 수 없는 level은 info 글자 색을 쓰고, 빈 model에는 기록을 표시하지 않아요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.ScrollView {
    id: control
    property var model: []
    property bool followTail: true
    implicitWidth: Theme.logViewWidth
    implicitHeight: Theme.logViewHeight
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
            padding: Theme.logLinePadding
            wrapMode: Text.Wrap
        }
    }
}
