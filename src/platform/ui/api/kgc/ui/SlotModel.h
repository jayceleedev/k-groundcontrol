// [0.0.2 T21] specs/features/0.0.2-theme.md
#pragma once
#include <QAbstractListModel>
namespace kgc::ui {
/** 기여 목록이 같은 빈 목록 동작을 공유하게 해요.
 * 입력: 부모 객체.
 * 출력: 없음.
 * 실패: 없음.
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
    /** 현재 등록된 기여 개수를 알려줘요.
     * 입력: 부모 인덱스.
     * 출력: 현재는 0.
     * 실패: 없음.
     */
    int rowCount(const QModelIndex& = {}) const override { return 0; }
    /** 현재는 기여가 없으므로 빈 값을 돌려줘요.
     * 입력: 인덱스와 역할.
     * 출력: 빈 값.
     * 실패: 없음.
     */
    QVariant data(const QModelIndex&, int) const override { return {}; }
};
}
