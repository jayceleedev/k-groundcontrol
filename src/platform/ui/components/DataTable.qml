// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 열 이름과 선택한 행을 구분하는 표예요.
 * 입력: columns는 제목 목록, rows는 행 목록이며 currentIndex, sortColumn, sortAscending과 sortRequested를 사용해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Column {
    id: table
    property var columns: []
    property var rows: []
    property int currentIndex: -1
    property int sortColumn: -1
    property bool sortAscending: true
    signal sortRequested(int column, bool ascending)
    width: Theme.languageWidth * 2
    Row {
        width: parent.width
        Repeater {
            model: table.columns
            ActionButton {
                required property int index
                required property var modelData
                width: table.width / Math.max(1, table.columns.length)
                text: modelData + (table.sortColumn === index ? (table.sortAscending ? " ↑" : " ↓") : "")
                kind: "secondary"
                textColor: Theme.inkMuted
                onClicked: { table.sortAscending = table.sortColumn === index ? !table.sortAscending : true; table.sortColumn = index; table.sortRequested(index, table.sortAscending) }
            }
        }
    }
    Repeater {
        model: table.rows
        Rectangle {
            id: row
            required property int index
            required property var modelData
            width: table.width
            height: Theme.menuHeight
            color: table.currentIndex === index ? Theme.infoSoft : index % 2 ? Theme.surfaceAlt : Theme.surface
            Row {
                anchors.fill: parent
                Repeater {
                    model: row.modelData
                    Text { required property var modelData; width: table.width / Math.max(1, table.columns.length); height: row.height; text: modelData; color: Theme.ink; font.pixelSize: Theme.fontSize; verticalAlignment: Text.AlignVCenter; leftPadding: Theme.inset; elide: Text.ElideRight }
                }
            }
            TapHandler { onTapped: table.currentIndex = row.index }
        }
    }
}
