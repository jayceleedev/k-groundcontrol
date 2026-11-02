// [0.3.0 6.8] specs/features/0.3.0-params.md
/** 왼쪽 묶음 목록이에요.
 * 입력: ParamsController.groups와 groupKey예요.
 * 출력: 줄을 누르면 ParamsController.groupKey를 바꿔요.
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
    model: ParamsController.groups
    Basic.ScrollBar.vertical: Basic.ScrollBar { implicitWidth: Theme.scrollBarWidth }
    delegate: Rectangle {
        id: line
        required property var modelData
        readonly property string key: modelData.key
        readonly property bool chosen: ParamsController.groupKey === key
        objectName: "paramsGroup_" + key
        width: list.width - Theme.scrollGutter
        height: Theme.paramRowHeight
        radius: Theme.radiusSmall
        color: chosen ? Theme.infoSoft : "transparent"
        Text {
            anchors.left: parent.left
            anchors.right: count.left
            anchors.leftMargin: Theme.inset
            anchors.rightMargin: Theme.inset
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight
            text: {
                // 묶음 제목도 언어가 바뀌면 다시 번역되도록 이 값을 의존성으로 읽어요.
                const language = translator.language
                return line.key === "*" ? qsTr("전체") : line.key === "" ? qsTr("기타") : ParamsController.groupLabel(line.key)
            }
            color: Theme.ink
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
        }
        Text {
            id: count
            anchors.right: parent.right
            anchors.rightMargin: Theme.inset
            anchors.verticalCenter: parent.verticalCenter
            text: line.key === "*" ? ParamsController.paramCount : line.modelData.count
            color: line.chosen ? Theme.ink : Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
        }
        TapHandler { onTapped: ParamsController.groupKey = line.key }
    }
}
