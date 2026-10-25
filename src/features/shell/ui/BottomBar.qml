// [0.0.2 T8 T14 T18] specs/features/0.0.2-theme.md
import QtQuick
import Kgc.Ui
GlassSurface {
    objectName: "bottomBar"
    height: Theme.bottomBarHeight
    solidColor: Theme.region
    Repeater { model: statusChipSlot; delegate: Item {} }
}
