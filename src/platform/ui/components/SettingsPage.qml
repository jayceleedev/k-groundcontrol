// [0.0.2 T25] specs/features/0.0.2-theme.md
/** 카테고리와 설정 항목을 일정한 비율로 나눠 보여줘요.
 * 입력: categories는 id·title 목록, currentIndex는 선택 위치이며 기본 자식은 오른쪽 항목이에요.
 * 출력: currentCategory로 선택 식별자를 내고 항목을 스크롤할 수 있어요.
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
    onCurrentIndexChanged: content.contentItem.contentY = 0
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
        rightPadding: Theme.inset
        contentWidth: availableWidth
        clip: true
        Column {
            id: sections
            width: content.availableWidth
            spacing: Theme.panelInset
        }
    }]
}
