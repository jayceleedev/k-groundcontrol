// [0.0.2 T12] specs/features/0.0.2-theme.md
// [0.0.3 T4 T5 T6] specs/features/0.0.3-windows.md
/** 수준별 색을 가진 기록을 끝까지 따라가며 보여줘요.
 * 입력: model은 text, level(info, debug, warn, error) 목록이며 followTail을 받아요.
 * 출력: atEnd는 끝 여부, positionAtEnd()는 끝 이동, userScrolled(atEnd)는 사용자 이동만 알려줘요.
 * 실패: 모르는 수준은 기본 글자 색을 쓰고, 빈 목록에는 기록을 표시하지 않아요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.ScrollView {
    id: control
    property var model: []
    property bool followTail: true
    readonly property bool atEnd: view.atYEnd
    signal userScrolled(bool atEnd)
    function positionAtEnd() { view.forceLayout(); view.positionViewAtEnd() }
    function followInserted() {
        if (followTail) Qt.callLater(function() { if (control.followTail) control.positionAtEnd() })
    }
    implicitWidth: Theme.logViewWidth
    implicitHeight: Theme.logViewHeight
    clip: true
    background: Rectangle { color: Theme.surfaceAlt; border.color: Theme.line; radius: Theme.radiusSmall }
    Basic.ScrollBar.vertical: Basic.ScrollBar {
        id: bar
        onPressedChanged: if (!pressed) control.userScrolled(view.atYEnd)
        onPositionChanged: if (pressed) control.userScrolled(view.atYEnd)
    }
    Connections {
        target: control.model && control.model.rowsInserted !== undefined ? control.model : null
        function onRowsInserted() { control.followInserted() }
    }
    contentItem: ListView {
        id: view
        model: control.model
        onCountChanged: control.followInserted()
        onMovementStarted: control.userScrolled(atYEnd)
        onMovementEnded: control.userScrolled(atYEnd)
        onContentYChanged: if (moving || bar.pressed) control.userScrolled(atYEnd)
        delegate: Text {
            required property var model
            readonly property var record: model.modelData !== undefined ? model.modelData : model
            width: view.width
            text: record.text
            color: record.level === "error" ? Theme.danger : record.level === "warn" ? Theme.warn : record.level === "debug" ? Theme.inkMuted : Theme.ink
            font.family: Theme.monoFamily
            font.pixelSize: Theme.fontSize
            padding: Theme.logLinePadding
            wrapMode: Text.Wrap
        }
    }
}
