// [0.0.2 T8 T12] specs/features/0.0.2-theme.md
// [0.0.4 T45] specs/features/0.0.4-righttabs.md
/** 제목과 내용을 같은 바탕 위에 묶어요.
 * 입력: title, source와 기본 자식 목록에 내용을 넘겨요.
 * 출력: title과 content 자식들을 GlassSurface 위에 세로로 배치해요.
 * 실패: source가 없으면 지도 흐림 없이 유리 색과 광택만 남아요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.Control {
    id: control
    property string title
    property Item source: null
    default property alias content: body.data
    padding: Theme.panelInset
    implicitWidth: body.implicitWidth + padding * 2
    implicitHeight: column.implicitHeight + padding * 2
    background: GlassSurface { source: control.source; radius: Theme.radiusMedium; clip: false }
    contentItem: Column {
        id: column
        spacing: Theme.inset
        Text { text: control.title; visible: text.length > 0; color: Theme.ink; font.pixelSize: Theme.titleSize; font.bold: true }
        Column { id: body; width: parent.width; spacing: Theme.inset }
    }
}
