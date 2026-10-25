// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 제목과 내용을 같은 바탕 위에 묶어요.
 * 입력: title, source와 기본 자식 목록에 내용을 넘겨요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
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
    background: GlassSurface { source: control.source; radius: Theme.radiusMedium }
    contentItem: Column {
        id: column
        spacing: Theme.inset
        Text { text: control.title; visible: text.length > 0; color: Theme.ink; font.pixelSize: Theme.titleSize; font.bold: true }
        Column { id: body; width: parent.width; spacing: Theme.inset }
    }
}
