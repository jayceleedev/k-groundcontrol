// [0.0.3 T21] specs/features/0.0.3-windows.md
// [0.0.2 T8 T14 T18] specs/features/0.0.2-theme.md
// [0.0.4 T38] specs/features/0.0.4-righttabs.md
import QtQuick
import QtQuick.Controls
import Kgc.Ui
GlassSurface {
    id: bar
    objectName: "topBar"
    height: Theme.topBarHeight
    solidColor: Theme.region
    IconButton {
        id: appIcon
        objectName: "appIcon"
        anchors.left: parent.left
        anchors.leftMargin: Theme.inset
        anchors.verticalCenter: parent.verticalCenter
        width: Theme.appIconSize
        height: Theme.appIconSize
        padding: Theme.appIconPadding
        hint: qsTr("정보")
        contentItem: IconGlyph { name: "app"; color: Theme.accent }
        onClicked: navigator.open("appSettings", "about")
    }
    // 가운데 조작 띠는 앱 아이콘 바로 뒤에서 시작하고, 오른쪽 묶음(포트·속도·연결·벨·AI)은 오른쪽 끝에 붙어요.
    Row {
        anchors.left: appIcon.right
        anchors.leftMargin: Theme.inset
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.inset
        Repeater { model: topbarSlot; delegate: TopbarEntry { area: "centre" } }
    }
    Row {
        anchors.right: parent.right
        anchors.rightMargin: Theme.inset
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.inset
        Repeater { model: topbarSlot; delegate: TopbarEntry { area: "right" } }
    }
}
