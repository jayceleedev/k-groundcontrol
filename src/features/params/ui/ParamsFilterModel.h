// [0.4.2 5.5] specs/features/0.4.2-param-tree.md
// [0.3.0 6.8] specs/features/0.3.0-params.md
#pragma once
#include <kgc/params/Params.h>
#include <QSortFilterProxyModel>
#include <QStringList>
namespace kgc::params {
/** 고른 트리 마디·검색어·토글을 AND로 걸러 표에 보일 행만 남겨요.
 * 입력: Params 묶음과 거름 조건이에요. 원본이 이미 자연순이라 정렬은 하지 않아요.
 * 출력: 조건에 맞는 행이에요.
 * 실패: 없어요.
 */
class ParamsFilterModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    /** 입력: 수명을 함께할 parent예요. 출력: 원본 없는 모델이에요. 실패: 없어요. */
    explicit ParamsFilterModel(QObject* parent);
    /** 입력: 원본 params이며 없어도 돼요. 출력: 원본을 바꿔요. 실패: 없어요. */
    void setParams(Params* params);
    /** 입력: 고른 마디의 key이며 "*"는 전체예요. 출력: underParamNode가 거짓인 행을 숨기도록 다시 걸러요. 실패: 없어요. */
    void setNodeKey(const QString& key);
    /** 입력: 검색 낱말 terms예요. 출력: 다시 걸러요. 실패: 없어요. */
    void setTerms(const QStringList& terms);
    /** 입력: 대기값 있는 행만 볼지예요. 출력: 다시 걸러요. 실패: 없어요. */
    void setPendingOnly(bool value);
    /** 입력: 기본값과 다른 행만 볼지예요. 출력: 다시 걸러요. 실패: 없어요. */
    void setNonDefaultOnly(bool value);
protected:
    /** 입력: 원본 행 row예요. 출력: 조건에 맞으면 참이에요. 실패: 없어요. */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;
private:
    Params* params_ = nullptr;
    QString nodeKey_ = QStringLiteral("*");
    QStringList terms_;
    bool pendingOnly_ = false;
    bool nonDefaultOnly_ = false;
};
}
