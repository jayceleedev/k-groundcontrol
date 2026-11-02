// [0.3.0 5.5 6.8-1] specs/features/0.3.0-params.md
#pragma once
#include <kgc/params/ParamMeta.h>
#include <QAbstractListModel>
#include <QHash>
#include <QSet>
#include <QStringList>
#include <cstdint>
#include <optional>
namespace kgc::params {
class ParamService;
/** 표의 한 행이에요.
 * 입력: PARAM_VALUE에서 온 이름·값·형과 사용자의 대기값, 계산한 묶음이에요.
 * 출력: 이름, FC 값 value, MAV_PARAM_TYPE 번호 type, 대기값 pending, 묶음 group이에요.
 * 실패: 없어요.
 */
struct Param {
    QString name;
    double value = 0.0;
    std::uint8_t type = 0;
    std::optional<double> pending;
    QString group;
};
/** 기체의 파라미터 값 전체와 메타데이터를 드는 유일한 묶음이에요(BASE §5).
 * 입력: ParamService만 쓰는 쓰기 함수예요.
 * 출력: 자연순으로 정렬한 행 목록과 화면 역할이에요.
 * 실패: 없는 이름의 조회는 nullptr예요.
 */
class Params : public QAbstractListModel {
    Q_OBJECT
public:
    /** 표 역할 번호예요. 이름은 roleNames가 정해요. */
    enum Role {
        NameRole = Qt::UserRole + 1, ValueRole, ValueTextRole, TypeRole, PendingRole, PendingTextRole,
        HasPendingRole, IsDefaultRole, HasDefaultRole, DefaultTextRole, UnitsRole, RangeTextRole,
        OptionTextRole, TitleRole, DescriptionRole, GroupRole, ReadOnlyRole, RebootRequiredRole, KindRole
    };
/** 빈 표를 만들어요.
 * 입력: 수명을 함께할 parent예요.
 * 출력: 행이 없는 묶음이에요. 언어가 바뀌면 rangeText를 다시 읽도록 알려요.
 * 실패: 없어요.
 */
    explicit Params(QObject* parent = nullptr);
/** 행 수를 읽어요.
 * 입력: 뿌리가 아닌 parent이면 0이에요.
 * 출력: 행 수예요.
 * 실패: 없어요.
 */
    int rowCount(const QModelIndex& parent = {}) const override;
/** 행의 역할 값을 읽어요.
 * 입력: 행 index와 Role 값 role이에요.
 * 출력: 역할의 값이며, 범위 밖이면 빈 값이에요.
 * 실패: 없어요.
 */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
/** 역할 이름을 돌려줘요.
 * 입력: 없어요.
 * 출력: QML에서 쓰는 역할 이름표예요.
 * 실패: 없어요.
 */
    QHash<int, QByteArray> roleNames() const override;
/** 이름으로 행을 찾아요.
 * 입력: 파라미터 이름 name이에요.
 * 출력: 행 포인터이며, 다음 변경 전까지만 유효해요.
 * 실패: 없으면 nullptr예요.
 */
    const Param* find(const QString& name) const;
/** 이름으로 메타데이터를 찾아요.
 * 입력: 파라미터 이름 name이에요.
 * 출력: 메타데이터 포인터이며, 다음 메타데이터 도착 전까지만 유효해요.
 * 실패: 없으면 nullptr예요.
 */
    const ParamMeta* meta(const QString& name) const;
/** 행 전체를 읽어요.
 * 입력: 없어요.
 * 출력: 자연순 행 목록이에요.
 * 실패: 없어요.
 */
    const QList<Param>& all() const;
/** 대기값 수를 읽어요.
 * 입력: 없어요.
 * 출력: 대기값이 있는 행 수예요.
 * 실패: 없어요.
 */
    int pendingCount() const;
/** 메타데이터에 기본값이 있는지 읽어요.
 * 입력: 없어요.
 * 출력: 기본값이 하나라도 있으면 참이에요.
 * 실패: 없어요.
 */
    bool hasDefaults() const;
signals:
/** 대기값 수가 바뀌었음을 알려요.
 * 입력: 없어요.
 * 출력: pendingCount를 다시 읽도록 알려요.
 * 실패: 없어요.
 */
    void pendingCountChanged();
/** 묶음 구성이 바뀌었음을 알려요.
 * 입력: 없어요.
 * 출력: 행의 group을 다시 읽도록 알려요.
 * 실패: 없어요.
 */
    void groupsChanged();
protected:
/** 언어 변경을 살펴요.
 * 입력: 앱에 온 사건이에요.
 * 출력: 언제나 거짓이라 사건을 막지 않아요. LanguageChange이면 rangeText의 dataChanged를 내요.
 * 실패: 없어요.
 */
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    friend class ParamService;
    int lowerBound(const QString& name) const;
    int rowOf(const QString& name) const;
    bool store(const QString& name, double value, std::uint8_t type, const QString& group);
    void setPending(const QString& name, std::optional<double> pending);
    void setMetadata(QHash<QString, ParamMeta> metadata);
    void setGroups(const QStringList& groups);
    void removeExcept(const QSet<QString>& names);
    void touch(int row);
    QList<Param> rows_;
    QHash<QString, ParamMeta> metadata_;
    int pendingCount_ = 0;
    bool hasDefaults_ = false;
};
}
