// [0.0.3 T2 T21] specs/features/0.0.3-windows.md
// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <kgc/ui/SlotModel.h>
#include <QPointer>

namespace kgc::ui {
/** 상단바의 왼쪽·가운데·오른쪽 자리에 기능 기여와 자리 표시를 순서대로 담아요.
 * 입력: 등록 항목과 자리 표시.
 * 출력: 순서대로 정렬된 목록과 영역·묶음·이름·아이콘·앞 구분선 역할.
 * 실패: 같은 식별자는 DuplicateId.
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
/** 아직 없는 기능의 상단바 자리를 비활성 단추로 잡아 둬요.
 * 입력: 식별자, 순서, 영역("left"·"centre"·"right"), 구분선 묶음 번호, 번역 문맥 "Bar"의 이름 원문, 아이콘 이름(없으면 빈 값),
 *       iconOnly(참이면 이름은 설명 풍선으로만 보이는 아이콘 단추).
 * 출력: 등록 결과.
 * 실패: 같은 식별자는 DuplicateId, 모르는 영역은 프로그래밍 오류로 종료해요.
 */
    kgc::app::errors::Result<void> registerPlaceholder(const QString& id, int order, const QString& area,
        int group, const QString& labelKey, const QString& icon, bool iconOnly = false);
/** 등록 값과 같은 영역 앞 항목과 묶음이 바뀌는지를 읽어요.
 * 입력: 인덱스와 역할.
 * 출력: 역할 값, separatorBefore는 앞 항목과 묶음이 다를 때 참.
 * 실패: 범위 밖이면 빈 값.
 */
    QVariant data(const QModelIndex& index, int role) const override;
/** 상단바 항목의 표시 역할을 알려줘요.
 * 입력: 없음.
 * 출력: 공통 역할과 area·group·labelKey·icon·iconOnly·separatorBefore.
 * 실패: 없음.
 */
    QHash<int, QByteArray> roleNames() const override;
};
/** 하단바 왼쪽·오른쪽 자리에 상태 칩 기여와 자리 표시를 순서대로 담아요.
 * 입력: 등록 항목과 자리 표시.
 * 출력: 순서대로 정렬된 목록과 side·labelKey 역할.
 * 실패: 같은 식별자는 DuplicateId.
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
/** 아직 없는 기능의 하단 칩 자리를 흐린 칩으로 잡아 둬요.
 * 입력: 식별자, 순서, 쪽("left"·"right"), 번역 문맥 "Bar"의 이름 원문.
 * 출력: 등록 결과.
 * 실패: 같은 식별자는 DuplicateId, 모르는 쪽은 프로그래밍 오류로 종료해요.
 */
    kgc::app::errors::Result<void> registerPlaceholder(const QString& id, int order, const QString& side,
        const QString& labelKey);
/** 하단 칩의 표시 역할을 알려줘요.
 * 입력: 없음.
 * 출력: 공통 역할과 side·labelKey.
 * 실패: 없음.
 */
    QHash<int, QByteArray> roleNames() const override;
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
