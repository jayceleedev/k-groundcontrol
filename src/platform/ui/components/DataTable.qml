// [0.0.2 T12] specs/features/0.0.2-theme.md
/** 열 이름과 선택한 행을 구분하는 표예요.
 * 입력: columns는 제목 목록, rows는 행 목록이며 currentIndex, sortColumn, sortAscending과 sortRequested를 사용해요.
 * 출력: 선택한 행의 currentIndex와 머리줄의 sortRequested(column, ascending)를 내요.
 * 실패: 정렬은 직접 하지 않아요. 행 길이와 열 개수의 일치는 호출자가 보장해요.
 */
import QtQuick
import Kgc.Ui

Column {
    id: table
    property var columns: []
    property var rows: []
    property int currentIndex: -1
    property int sortColumn: -1
    property bool sortAscending: true
    signal sortRequested(int column, bool ascending)
    width: Theme.dataTableWidth
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
            height: Theme.dataRowHeight
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
