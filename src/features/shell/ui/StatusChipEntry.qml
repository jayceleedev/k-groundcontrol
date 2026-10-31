// [0.1.0 T7] specs/features/0.1.0-link.md
/** 기여 화면과 자리 표시가 같은 칩 높이를 쓰도록 감싸요.
 * 입력: side와 슬롯 모델의 id·side·labelKey·component.
 * 출력: 선택한 쪽의 기여 화면 또는 흐린 자리 표시예요.
 * 실패: 없음.
 */
import QtQuick
import Kgc.Ui
Item {
    id: entry
    required property var model
    property string side
    visible: model.side === side
    height: Theme.bottomBarHeight - Theme.inset
    width: content.implicitWidth
    Loader {
        id: content
        active: entry.model.side === entry.side
        height: parent.height
        source: entry.model.component
        sourceComponent: entry.model.component.toString() ? null : placeholder
    }
    Component {
        id: placeholder
        Chip {
            objectName: "statusChip_" + entry.model.id
            level: "idle"
            text: { translator.language; return qsTranslate("Bar", entry.model.labelKey) }
        }
    }
}
