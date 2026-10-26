// [0.0.3 T21] specs/features/0.0.3-windows.md
/** 하단 상태 칩 슬롯 한 항목을 그 쪽에 그려요. 아직 기능이 없는 자리는 흐린 칩으로 보여요.
 * 입력: side(이 줄이 그리는 쪽)와 슬롯 모델의 id·side·labelKey.
 * 출력: 쪽이 맞으면 칩을, 아니면 아무것도 보이지 않아요.
 * 실패: 없음.
 */
import QtQuick
import Kgc.Ui
Chip {
    id: chip
    required property var model
    property string side
    visible: model.side === side
    objectName: "statusChip_" + model.id
    level: "idle"
    height: Theme.bottomBarHeight - Theme.inset
    text: { translator.language; return qsTranslate("Bar", model.labelKey) }
}
