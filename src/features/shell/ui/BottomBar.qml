// [0.0.2 T8 T14 T18] specs/features/0.0.2-theme.md
// [0.0.3 T2] specs/features/0.0.3-windows.md
import QtQuick
import Kgc.Ui
GlassSurface {
    objectName: "bottomBar"
    height: Theme.bottomBarHeight
    solidColor: Theme.region
    Repeater { model: statusChipSlot; delegate: Item {} }
    Row {
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
                name: model.icon
                hint: { translator.language; return qsTranslate("BottomAction", model.hintKey) }
                onClicked: bottomActionSlot.trigger(model.id)
            }
        }
    }
}
