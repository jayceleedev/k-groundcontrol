// [0.0.1 T4–T9] specs/features/0.0.1-shell.md
import QtQuick
import Kgc.Ui
Rectangle {
    objectName: "bottomBar"
    height: Theme.bottomBarHeight
    color: Theme.region
    border.color: Theme.line
    Repeater { model: statusChipSlot; delegate: Item {} }
}
