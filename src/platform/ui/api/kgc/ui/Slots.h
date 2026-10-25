// [0.0.3 T2] specs/features/0.0.3-windows.md
// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <kgc/ui/SlotModel.h>
#include <QPointer>

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
/** 하단바 단추의 표시와 호출 대상을 함께 등록해요.
 * 입력: 단추 정보와 수신 객체.
 * 출력: 정렬된 단추 목록.
 * 실패: 같은 식별자는 DuplicateId.
 */
class BottomActionSlot : public SlotModel {
    Q_OBJECT
public:
    /** 단추 목록을 만들어요.
     * 입력: 부모 객체.
     * 출력: 없음.
     * 실패: 없음.
     */
    explicit BottomActionSlot(QObject* parent = nullptr);
    /** 단추를 등록하고 호출할 메서드를 고정해요.
     * 입력: 식별자, 순서, 아이콘, 힌트 원문, 수신 객체와 메서드 이름.
     * 출력: 등록 결과.
     * 실패: 같은 식별자는 DuplicateId, 수신 객체나 인자 없는 메서드가 없으면 프로그래밍 오류로 종료해요.
     */
    kgc::app::errors::Result<void> registerAction(const QString& id, int order,
        const QString& icon, const QString& hintKey, QObject* receiver, const char* method);
    /** 등록한 단추의 동작을 호출해요.
     * 입력: 단추 식별자.
     * 출력: 없음.
     * 실패: 없는 식별자나 사라진 수신 객체는 무시해요.
     * 흐름:
     * 1. QMetaObject::invokeMethod — 등록된 컨트롤러 메서드에 넘겨요.
     */
    Q_INVOKABLE void trigger(const QString& id);
    /** 단추의 표시 역할을 알려줘요.
     * 입력: 없음.
     * 출력: 공통 역할과 icon·hintKey.
     * 실패: 없음.
     */
    QHash<int, QByteArray> roleNames() const override;
private:
    QHash<QString, std::pair<QPointer<QObject>, QByteArray>> actions_;
};
}
