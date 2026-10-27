// [0.0.4 T30 T31] specs/features/0.0.4-righttabs.md
// [0.0.2 T25] specs/features/0.0.2-theme.md
/** 카테고리와 설정 항목을 일정한 비율로 나눠 보여줘요.
 * 입력: categories는 id·title 목록, currentIndex는 선택 위치이며 기본 자식 Section의 category로 항목을 나눠요.
 * 출력: currentCategory로 선택 식별자를 내고 해당 항목의 보이기와 머리글을 정해요.
 * 실패: 빈 목록이나 범위 밖 선택에는 빈 식별자를 내요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Item {
    id: page
    property var categories: []
    property int currentIndex: 0
    readonly property string currentCategory: categories[currentIndex]?.id ?? ""
    default property alias contentData: sections.data
    onCurrentCategoryChanged: updateSections()
    onCategoriesChanged: updateSections()
    Component.onCompleted: updateSections()
    /** 선택한 카테고리의 항목을 즉시 배치해요.
     * 입력: categories와 자식 Section의 category를 읽어요.
     * 출력: 보이는 항목과 머리글을 갱신하고 스크롤을 처음으로 돌려요.
     * 실패: 목록에 없는 카테고리의 머리글은 비워요.
     */
    function updateSections() {
        for (const section of sections.children) {
            if (!(section instanceof Section)) continue
            section.visible = Qt.binding(() => section.category === page.currentCategory)
            section.text = Qt.binding(() => page.categories.find(entry => entry.id === section.category)?.title ?? "")
            section.forceLayout()
        }
        // 다음 프레임 전에도 새 항목의 좌표로 클릭을 받도록 배치를 끝내요.
        sections.forceLayout()
        content.contentItem.contentY = 0
    }
    children: [ListView {
        id: categoryList
        objectName: "settingsCategoryList"
        width: page.width / 4
        height: page.height
        clip: true
        model: page.categories
        currentIndex: page.currentIndex
        spacing: Theme.choiceGap
        activeFocusOnTab: true
        keyNavigationEnabled: false
        Keys.onUpPressed: page.currentIndex = Math.max(0, page.currentIndex - 1)
        Keys.onDownPressed: page.currentIndex = Math.min(count - 1, page.currentIndex + 1)
        delegate: Basic.ItemDelegate {
            required property int index
            required property var modelData
            objectName: "settingsCategory_" + modelData.id
            width: categoryList.width - Theme.inset
            height: Theme.controlHeight
            padding: Theme.inset
            focusPolicy: Qt.NoFocus
            highlighted: page.currentIndex === index
            text: modelData.title
            onClicked: { categoryList.forceActiveFocus(); page.currentIndex = index }
            contentItem: Text {
                text: parent.text
                color: parent.highlighted ? Theme.accentInk : Theme.ink
                font.pixelSize: Theme.fontSize
                font.family: Theme.fontFamily
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            background: Rectangle {
                color: parent.highlighted ? Theme.accent : parent.hovered ? Theme.surfaceAlt : "transparent"
                radius: Theme.radiusSmall
                border.color: categoryList.activeFocus && parent.highlighted ? Theme.focus : "transparent"
                border.width: Theme.lineWidth * 2
            }
        }
    }, Basic.ScrollView {
        id: content
        objectName: "settingsContent"
        x: categoryList.width
        width: page.width * 3 / 4
        height: page.height
        leftPadding: Theme.panelInset
        rightPadding: Theme.scrollGutter
        Basic.ScrollBar.vertical.implicitWidth: Theme.scrollBarWidth
        contentWidth: availableWidth
        clip: true
        Column {
            id: sections
            onChildrenChanged: page.updateSections()
            width: content.availableWidth
            spacing: Theme.panelInset
        }
    }]
}
