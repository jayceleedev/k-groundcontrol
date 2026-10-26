// [0.0.2 T8 T14 T18] specs/features/0.0.2-theme.md
// [0.0.4 T3 T4 T7 T10 T10a T13] specs/features/0.0.4-righttabs.md
// 셸은 우 탭의 주인을 모르고 RightTabSlot의 역할 값(placement·titleKey·component)만 읽어 배치해요.
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
GlassSurface {
    id: bar
    objectName: "rightBar"
    width: Theme.rightBarWidth
    solidColor: Theme.region
    // 등록부의 행을 계기 칸 항목 하나(topComponent)와 탭 목록(tabs: id·titleKey·component, order 순)으로 나눠 둬요.
    property url topComponent: ""
    property var tabs: []
    function collect() {
        let top = ""
        const list = []
        for (let i = 0; i < entries.count; ++i) {
            const entry = entries.objectAt(i)
            if (!entry) continue
            if (entry.placement === "top") top = entry.component
            else list.push({ id: entry.entryId, titleKey: entry.titleKey, component: entry.component })
        }
        topComponent = top
        tabs = list
    }
    Instantiator {
        id: entries
        model: rightTabSlot
        delegate: QtObject {
            required property var model
            readonly property string entryId: model.id
            readonly property string titleKey: model.titleKey
            readonly property url component: model.component
            readonly property string placement: model.placement
        }
        onObjectAdded: bar.collect()
        onObjectRemoved: bar.collect()
    }
    Rectangle {
        id: top
        objectName: "rightTop"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme.inset
        height: Theme.instrumentHeight
        color: Theme.surface
        border.color: Theme.line
        Loader { anchors.fill: parent; source: bar.topComponent }
    }
    TabStrip {
        id: tabStrip
        objectName: "rightTabs"
        anchors.top: top.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme.inset
        height: Theme.rightTabHeight
        fill: true
        buttonNames: bar.tabs.map(tab => "rightTab_" + tab.id)
        model: { translator.language; return bar.tabs.map(tab => qsTranslate("RightTab", tab.titleKey)) }
    }
    // 고른 탭의 화면을 싣는 칸이에요. 실린 화면의 높이는 max(자기 내용 높이, 칸 높이)이고, 내용이 칸보다 길 때만 세로로 스크롤해요.
    Flickable {
        id: content
        objectName: "rightTabContent"
        anchors.top: tabStrip.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        contentWidth: width
        contentHeight: page.height
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        interactive: contentHeight > height
        Loader {
            id: page
            width: content.width
            height: Math.max(item ? item.implicitHeight : 0, content.height)
            source: bar.tabs.length > 0 ? bar.tabs[Math.min(tabStrip.currentIndex, bar.tabs.length - 1)].component : ""
            // 탭을 바꾸면 새 탭이 맨 위에서 시작해요.
            onLoaded: content.contentY = 0
        }
        // 스크롤바는 내용 위 오른쪽 Theme.inset 여백 안에 겹쳐 그려 내용 폭을 줄이지 않아요.
        Basic.ScrollBar.vertical: Basic.ScrollBar {
            objectName: "rightTabScrollBar"
            width: Theme.inset
            padding: 0
            policy: Basic.ScrollBar.AlwaysOn
            visible: content.contentHeight > content.height
            contentItem: Rectangle { radius: width / 2; color: Theme.lineStrong }
        }
    }
}
