// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <QAbstractListModel>

namespace kgc::ui {
/** 다음 기능이 기여할 빈 목록을 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class TopbarSlot : public QAbstractListModel {
    Q_OBJECT
public:
/** 빈 슬롯을 만들어요.
 * 입력: 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit TopbarSlot(QObject* parent = nullptr);
/** 기여 개수를 알려줘요.
 * 입력: 부모 인덱스.
 * 출력: 현재는 0.
 * 실패: 없음.
 */
    int rowCount(const QModelIndex& parent = {}) const override;
/** 기여 항목을 읽어요.
 * 입력: 인덱스와 역할.
 * 출력: 현재는 빈 값.
 * 실패: 없음.
 */
    QVariant data(const QModelIndex& index, int role) const override;
};
/** 다음 기능이 기여할 빈 목록을 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class StatusChipSlot : public QAbstractListModel {
    Q_OBJECT
public:
/** 빈 슬롯을 만들어요.
 * 입력: 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit StatusChipSlot(QObject* parent = nullptr);
/** 기여 개수를 알려줘요.
 * 입력: 부모 인덱스.
 * 출력: 현재는 0.
 * 실패: 없음.
 */
    int rowCount(const QModelIndex& parent = {}) const override;
/** 기여 항목을 읽어요.
 * 입력: 인덱스와 역할.
 * 출력: 현재는 빈 값.
 * 실패: 없음.
 */
    QVariant data(const QModelIndex& index, int role) const override;
};
/** 다음 기능이 기여할 빈 목록을 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class RightTabSlot : public QAbstractListModel {
    Q_OBJECT
public:
/** 빈 슬롯을 만들어요.
 * 입력: 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit RightTabSlot(QObject* parent = nullptr);
/** 기여 개수를 알려줘요.
 * 입력: 부모 인덱스.
 * 출력: 현재는 0.
 * 실패: 없음.
 */
    int rowCount(const QModelIndex& parent = {}) const override;
/** 기여 항목을 읽어요.
 * 입력: 인덱스와 역할.
 * 출력: 현재는 빈 값.
 * 실패: 없음.
 */
    QVariant data(const QModelIndex& index, int role) const override;
};
/** 다음 기능이 기여할 빈 목록을 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class MapLayerSlot : public QAbstractListModel {
    Q_OBJECT
public:
/** 빈 슬롯을 만들어요.
 * 입력: 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit MapLayerSlot(QObject* parent = nullptr);
/** 기여 개수를 알려줘요.
 * 입력: 부모 인덱스.
 * 출력: 현재는 0.
 * 실패: 없음.
 */
    int rowCount(const QModelIndex& parent = {}) const override;
/** 기여 항목을 읽어요.
 * 입력: 인덱스와 역할.
 * 출력: 현재는 빈 값.
 * 실패: 없음.
 */
    QVariant data(const QModelIndex& index, int role) const override;
};
}
