// [0.0.3 T17 T19] specs/features/0.0.3-windows.md
// [0.0.4 T25a T25b T25c T25d T28] specs/features/0.0.4-righttabs.md
// [0.0.2 T8 T14 T18] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls
import Kgc.Ui
GlassSurface {
    id: menu
    objectName: "leftMenu"
    // 끄는 동안에는 지역 값 dragWidth(min~max로 자름)로 그리고, 놓을 때 한 번만 저장해요(그 전엔 설정이 바뀌지 않아요).
    property bool dragging: false
    property real dragWidth: shellController.leftMenuWidth
    width: shellController.leftMenuCollapsed ? Theme.leftMenuCollapsedWidth
        : (dragging ? dragWidth : shellController.leftMenuWidth) * Theme.uiScale
    solidColor: Theme.region
    IconButton {
        id: collapse
        objectName: "leftMenuCollapse"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: Theme.inset
        width: Theme.frameButtonSize
        height: Theme.frameButtonSize
        padding: 0
        icon.width: Theme.frameIconSize
        icon.height: Theme.frameIconSize
        name: shellController.leftMenuCollapsed ? "layout-sidebar-left-expand" : "layout-sidebar-left-collapse"
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
                        // 이름이 메뉴 폭에 안 들어가면 끝을 …로 줄이고, 줄었으면 마우스를 올릴 때 전체 이름 힌트를 보여요.
                        contentItem: Row {
                            spacing: menuButton.spacing
                            IconGlyph {
                                source: menuButton.icon.source
                                color: menuButton.icon.color
                                width: menuButton.icon.width
                                height: menuButton.icon.height
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text {
                                id: menuLabel
                                objectName: "menuLabel"
                                visible: text.length > 0
                                width: Math.max(0, menuButton.availableWidth - menuButton.icon.width - menuButton.spacing)
                                elide: Text.ElideRight
                                text: menuButton.text
                                font: menuButton.font
                                color: menuButton.textColor
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                        Accessible.name: entry.menuTitle
                        HintTip {
                            text: entry.menuTitle
                            visible: menuButton.hovered && (shellController.leftMenuCollapsed || menuLabel.truncated)
                        }
                        onClicked: shellController.togglePanel(entry.panelId)
                    }
                    Item {
                        // 메뉴 표의 묶음이 1부터 차례라서 끝나는 묶음 번호가 구분선 순번과 같아요.
                        objectName: entry.model.separatorAfter ? "menuSeparator_" + entry.model.group : ""
                        visible: entry.model.separatorAfter
                        width: parent.width
                        height: Theme.menuSeparatorSpace
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width
                            height: Theme.lineWidth
                            color: Theme.lineStrong
                        }
                    }
                }
            }
        }
    }
    // 오른쪽 가장자리 손잡이: 끌면 좌 메뉴 폭이 따라 바뀌어요(접힌 동안엔 없어요).
    MouseArea {
        id: resizeHandle
        objectName: "leftMenuResizeHandle"
        property real startX: 0
        property real startWidth: 0
        visible: !shellController.leftMenuCollapsed
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: Theme.inset
        hoverEnabled: true
        cursorShape: Qt.SizeHorCursor
        // 손잡이가 폭과 함께 움직이므로 끈 거리는 창 좌표로 재요.
        onPressed: mouse => {
            startX = mapToItem(null, mouse.x, 0).x
            startWidth = shellController.leftMenuWidth
            menu.dragWidth = startWidth
            menu.dragging = true
        }
        onPositionChanged: mouse => {
            if (!menu.dragging) return
            const moved = (mapToItem(null, mouse.x, 0).x - startX) / Theme.uiScale
            menu.dragWidth = Math.max(shellController.leftMenuWidthMin,
                Math.min(shellController.leftMenuWidthMax, Math.round(startWidth + moved)))
        }
        onReleased: {
            if (!menu.dragging) return
            shellController.leftMenuWidth = menu.dragWidth
            menu.dragging = false
        }
        onCanceled: menu.dragging = false
    }
}
