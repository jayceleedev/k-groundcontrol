// [0.0.2 T8 T14 T18] specs/features/0.0.2-theme.md
// [0.0.3 T2 T21] specs/features/0.0.3-windows.md
import QtQuick
import Kgc.Ui
GlassSurface {
    objectName: "bottomBar"
    height: Theme.bottomBarHeight
    solidColor: Theme.region
    Row {
        anchors.left: parent.left
        anchors.leftMargin: Theme.inset
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.choiceGap
        Repeater { model: statusChipSlot; delegate: StatusChipEntry { side: "left" } }
    }
    // 오른쪽 칩(트래커·GCS GPS)은 맨 오른쪽 단추들 바로 왼쪽에 붙어요.
    Row {
        anchors.right: actions.left
        anchors.rightMargin: Theme.inset
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.choiceGap
        Repeater { model: statusChipSlot; delegate: StatusChipEntry { side: "right" } }
    }
    Row {
        id: actions
        anchors.right: parent.right
        anchors.rightMargin: Theme.inset
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.choiceGap
        Repeater {
            model: bottomActionSlot
            delegate: IconButton {
                required property var model
                objectName: "bottomAction_" + model.id
                width: Theme.bottomBarHeight - Theme.inset
                height: Theme.bottomBarHeight - Theme.inset
                padding: (height - Theme.iconSize) / 2
                name: model.icon
                hint: { translator.language; return qsTranslate("BottomAction", model.hintKey) }
                onClicked: bottomActionSlot.trigger(model.id)
            }
        }
    }
}
