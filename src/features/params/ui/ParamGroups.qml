// [0.4.2 6.1 6.2] specs/features/0.4.2-param-tree.md
// [0.3.0 6.8] specs/features/0.3.0-params.md
/** 왼쪽 파라미터 트리예요.
 * 입력: ParamsController.tree의 보이는 줄과 nodeKey예요.
 * 출력: 줄을 누르면 ParamsController.nodeKey를 바꾸고, 펼침 표시를 누르거나 가지 줄을 두 번 누르면 toggleNode를 불러요.
 * 실패: 없어요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import Kgc.Params

ListView {
    id: list
    objectName: "paramsGroups"
    clip: true
    boundsBehavior: Flickable.StopAtBounds
    model: ParamsController.tree
    Basic.ScrollBar.vertical: Basic.ScrollBar { implicitWidth: Theme.scrollBarWidth }
    delegate: Rectangle {
        id: line
        required property string key
        required property bool branch
        required property int depth
        required property bool expanded
        required property int count
        readonly property bool chosen: ParamsController.nodeKey === key
        readonly property bool hasCount: key === "*" || branch
        objectName: "paramsNode_" + key
        width: list.width - Theme.scrollGutter
        height: Theme.paramTreeRowHeight
        radius: Theme.radiusSmall
        color: chosen ? Theme.infoSoft : "transparent"
        // 줄을 고르는 손잡이예요. 펼침 표시의 손잡이가 누름을 차지하면 이 손잡이는 반응하지 않아요.
        TapHandler {
            onTapped: ParamsController.nodeKey = line.key
            onDoubleTapped: if (line.branch) ParamsController.toggleNode(line.key)
        }
        // 들여쓰기 뒤의 펼침 표시 자리예요. 잎과 전체 줄은 같은 폭의 빈 자리라 글자가 깊이마다 나란히 서요.
        Item {
            id: toggle
            objectName: line.branch ? "paramsNodeToggle_" + line.key : ""
            x: line.depth * Theme.paramTreeIndent
            width: Theme.flatIconButtonSize
            height: parent.height
            IconGlyph {
                anchors.centerIn: parent
                width: Theme.flatIconSize
                height: Theme.flatIconSize
                visible: line.branch
                name: line.expanded ? "chevron-down" : "chevron-right"
                color: Theme.inkMuted
            }
            TapHandler {
                enabled: line.branch
                gesturePolicy: TapHandler.ReleaseWithinBounds
                onTapped: ParamsController.toggleNode(line.key)
            }
        }
        Text {
            anchors.left: toggle.right
            anchors.right: line.hasCount ? countText.left : parent.right
            anchors.rightMargin: Theme.inset
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight
            text: {
                // 묶음 제목도 언어가 바뀌면 다시 번역되도록 이 값을 의존성으로 읽어요.
                const language = translator.language
                return line.key === "*" ? qsTr("전체") : ParamsController.nodeLabel(line.key, line.depth)
            }
            color: Theme.ink
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
        }
        Text {
            id: countText
            visible: line.hasCount
            anchors.right: parent.right
            anchors.rightMargin: Theme.inset
            anchors.verticalCenter: parent.verticalCenter
            text: line.key === "*" ? ParamsController.paramCount : line.count
            color: line.chosen ? Theme.ink : Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
        }
    }
}
