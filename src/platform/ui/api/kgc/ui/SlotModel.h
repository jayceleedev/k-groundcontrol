// [0.0.3 T2 T15 T21] specs/features/0.0.3-windows.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <QAbstractListModel>
#include <QVariantMap>
namespace kgc::ui {
/** 기여의 식별자와 순서, 슬롯별 표시값을 담아요.
 * 입력: 식별자와 정렬 순서와 추가 역할 값.
 * 출력: 등록 항목.
 * 실패: 없음.
 */
struct SlotEntry { QString id; int order; QVariantMap values; };
/** 기여 목록의 등록과 정렬, 중복 거부를 한 곳에서 맡아요.
 * 입력: 부모 객체와 등록 항목.
 * 출력: 정렬된 목록.
 * 실패: 같은 식별자는 DuplicateId.
 */
class SlotModel : public QAbstractListModel {
    Q_OBJECT
public:
    /** 기여 목록의 수명을 부모와 함께 관리해요.
     * 입력: 부모 객체.
     * 출력: 없음.
     * 실패: 없음.
     */
    explicit SlotModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}
    /** 새 기여를 정렬된 자리에 등록해요.
     * 입력: 등록 항목.
     * 출력: 성공 결과.
     * 실패: 식별자가 겹치면 DuplicateId.
     */
    kgc::app::errors::Result<void> add(SlotEntry entry);
    /** 현재 등록된 기여 개수를 알려줘요.
     * 입력: 부모 인덱스.
     * 출력: 루트 항목 수.
     * 실패: 없음.
     */
    int rowCount(const QModelIndex& parent = {}) const override;
    /** 화면에서 요구한 기여 값을 읽어요.
     * 입력: 인덱스와 역할.
     * 출력: 역할 값.
     * 실패: 범위 밖이면 빈 값.
     */
    QVariant data(const QModelIndex& index, int role) const override;
    /** 기여의 공통 역할을 알려줘요.
     * 입력: 없음.
     * 출력: id와 order 역할.
     * 실패: 없음.
     */
    QHash<int, QByteArray> roleNames() const override;
protected:
    /** 파생 슬롯이 이웃 항목을 보고 역할 값을 정할 수 있게 등록 항목을 읽어요.
     * 입력: 행 번호.
     * 출력: 그 행의 등록 항목.
     * 실패: 범위 밖 행은 부르는 쪽의 프로그래밍 오류예요.
     */
    const SlotEntry& entryAt(int row) const { return entries_[row]; }
private:
    QList<SlotEntry> entries_;
    mutable QHash<int, QByteArray> roles_;
};
}
