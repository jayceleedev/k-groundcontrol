// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <kgc/ui/SlotModel.h>

namespace kgc::ui {
/** 다음 기능이 기여할 빈 목록을 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class TopbarSlot : public SlotModel {
    Q_OBJECT
public:
/** 빈 슬롯을 만들어요.
 * 입력: 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit TopbarSlot(QObject* parent = nullptr);
};
/** 다음 기능이 기여할 빈 목록을 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class StatusChipSlot : public SlotModel {
    Q_OBJECT
public:
/** 빈 슬롯을 만들어요.
 * 입력: 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit StatusChipSlot(QObject* parent = nullptr);
};
/** 다음 기능이 기여할 빈 목록을 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class RightTabSlot : public SlotModel {
    Q_OBJECT
public:
/** 빈 슬롯을 만들어요.
 * 입력: 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit RightTabSlot(QObject* parent = nullptr);
};
/** 다음 기능이 기여할 빈 목록을 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class MapLayerSlot : public SlotModel {
    Q_OBJECT
public:
/** 빈 슬롯을 만들어요.
 * 입력: 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit MapLayerSlot(QObject* parent = nullptr);
};
}
