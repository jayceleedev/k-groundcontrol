// [0.0.2 T8 T14 T18] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls
import Kgc.Ui
GlassSurface {
    id: menu
    objectName: "leftMenu"
    width: shellController.leftMenuCollapsed ? Theme.leftMenuCollapsedWidth : Theme.leftMenuWidth
    solidColor: Theme.region
    IconButton {
        id: collapse
        objectName: "leftMenuCollapse"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: Theme.inset
        width: Theme.menuHeight
        height: Theme.menuHeight
        name: shellController.leftMenuCollapsed ? "chevron-right" : "chevron-left"
        hint: shellController.leftMenuCollapsed ? qsTr("메뉴 펼치기") : qsTr("메뉴 접기")
        onClicked: shellController.leftMenuCollapsed = !shellController.leftMenuCollapsed
    }
    Flickable {
        anchors.top: collapse.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme.inset
        contentHeight: entries.height
        clip: true
        Column {
            id: entries
            width: parent.width
            spacing: Theme.menuGap
            Repeater {
                model: panelRegistry
                delegate: Column {
                    id: entry
                    required property var model
                    property string panelId: model.id
                    property string menuTitle: {
                        // 동적 메뉴 제목도 언어 변경 시 다시 번역되도록 이 속성을 바인딩 의존성으로 읽어요.
                        const language = translator.language
                        return qsTranslate("Menu", model.titleKey)
                    }
                    width: entries.width
                    spacing: Theme.menuGap
                    ActionButton {
                        id: menuButton
                        objectName: "menuItem_" + entry.panelId
                        width: parent.width
                        height: Theme.menuHeight
                        text: shellController.leftMenuCollapsed ? "" : entry.menuTitle
                        kind: shellController.activePanelId === entry.panelId ? "primary" : "secondary"
                        icon.source: entry.model.icon
                        icon.color: Theme.ink
                        Accessible.name: entry.menuTitle
                        HintTip { text: entry.menuTitle; visible: menuButton.hovered && shellController.leftMenuCollapsed }
                        onClicked: shellController.togglePanel(entry.panelId)
                    }
                    Rectangle {
                        // 메뉴 표의 묶음이 1·2·3 순서여서 끝나는 묶음 번호가 구분선 순번과 같아요.
                        objectName: entry.model.separatorAfter ? "menuSeparator_" + entry.model.group : ""
                        visible: entry.model.separatorAfter
                        width: parent.width
                        height: Theme.lineWidth
                        color: Theme.line
                    }
                }
            }
        }
    }
}
