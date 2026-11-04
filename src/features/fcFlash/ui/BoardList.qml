// [0.4.1 6.3] specs/features/0.4.1-board-match.md
/** 2단계의 보드 후보 목록을 테두리 상자 안에 그려요.
 * 입력: rows(항목 {name, index, boardId, matched, other}, 그린 순서 그대로)와 펼칠 행 수 visibleRows예요.
 * 출력: 같은 보드 ID 후보는 고를 수 있는 RadioRow와 "보드 ID" 칩, 맞은 후보는 "인식됨" 칩을 더해요.
 *       다른 보드는 비활성 RadioRow(글자 inkMuted)와 idle 칩이에요. visibleRows보다 많으면 목록 안에서 스크롤해요.
 * 실패: 없어요. 다른 보드 행은 눌러도 선택이 바뀌지 않아요.
 * 흐름:
 * 1. FcFlashController.chooseCandidate — 같은 보드 ID 후보 행을 누르면 그 후보를 골라요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import Kgc.FcFlash

Rectangle {
    id: frame
    property var rows: []
    property int visibleRows: 8
    implicitHeight: boardList.height + Theme.lineWidth * 2
    color: Theme.surface
    radius: Theme.radiusMedium
    border.width: Theme.lineWidth
    border.color: Theme.line
    ListView {
        id: boardList
        objectName: "fcFlashBoardList"
        x: Theme.lineWidth
        y: Theme.lineWidth
        width: parent.width - Theme.lineWidth * 2
        // 다른 보드는 수백 개라서 행 8개 높이까지만 펼치고 그 뒤는 목록 안에서 스크롤해요.
        height: Math.min(count, frame.visibleRows) * Theme.controlHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        model: frame.rows
        Basic.ScrollBar.vertical: Basic.ScrollBar { implicitWidth: Theme.scrollBarWidth }
        delegate: Item {
            id: row
            required property var modelData
            required property int index
            width: ListView.view.width
            height: Theme.controlHeight
            RadioRow {
                id: radio
                objectName: row.modelData.other ? "fcFlashOther_" + row.modelData.name + "_" + row.modelData.boardId
                    : "fcFlashCandidate_" + row.modelData.index
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                width: Math.max(implicitWidth, parent.width - chips.width - Theme.inset)
                autoExclusive: false
                text: row.modelData.name
                enabled: !row.modelData.other
                checked: !row.modelData.other && row.modelData.index === FcFlashController.candidateIndex
                onClicked: FcFlashController.chooseCandidate(row.modelData.index)
                // 다른 보드 행은 고를 수 없지만 수백 행을 읽어야 해서 글자만 inkMuted로 바꿔요. 원은 비활성 모양 그대로예요.
                contentItem: Text {
                    text: radio.text
                    font: radio.font
                    color: row.modelData.other ? Theme.inkMuted : Theme.ink
                    verticalAlignment: Text.AlignVCenter
                }
            }
            Row {
                id: chips
                anchors.right: parent.right
                anchors.rightMargin: Theme.inset + (boardList.count > frame.visibleRows ? Theme.scrollBarWidth : 0)
                anchors.verticalCenter: parent.verticalCenter
                spacing: Theme.inset
                Chip { visible: row.modelData.matched; text: qsTr("인식됨"); level: "ok" }
                Chip {
                    text: row.modelData.other ? qsTr("보드 ID %1 · 구울 수 없음").arg(row.modelData.boardId)
                        : qsTr("보드 ID %1").arg(row.modelData.boardId)
                    level: row.modelData.other ? "idle" : "ok"
                }
            }
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: Theme.lineWidth
                color: Theme.line
                visible: row.index < boardList.count - 1
            }
        }
    }
}
