// [0.4.2 5.4] specs/features/0.4.2-param-tree.md
#pragma once
#include "ParamTree.h"
#include <QAbstractListModel>
#include <QHash>
#include <QSet>
#include <QStringList>
namespace kgc::params {
/** 왼쪽 파라미터 트리에서 지금 보이는 줄을 QML 목록에 내놓아요.
 * 입력: setNames로 받은 이름 목록과 toggle·expandTo·collapseAll로 바뀌는 펼침이에요.
 * 출력: 0번 줄은 전체("*"), 그 뒤는 visibleParamNodes가 고른 마디예요. 역할은 key·branch·depth·expanded·count예요.
 * 실패: 없어요.
 */
class ParamTreeModel : public QAbstractListModel {
    Q_OBJECT
public:
    /** 줄 역할 번호예요. 이름은 roleNames가 정하고, 번호 순서와 같아요. */
    enum Role { KeyRole = Qt::UserRole + 1, BranchRole, DepthRole, ExpandedRole, CountRole };
/** 빈 트리를 만들어요.
 * 입력: 수명을 함께할 parent예요.
 * 출력: 전체 줄 하나만 있는 모델이에요.
 * 실패: 없어요.
 */
    explicit ParamTreeModel(QObject* parent);
/** 줄 수를 읽어요.
 * 입력: 뿌리가 아닌 parent이면 0이에요.
 * 출력: 전체 줄 하나와 보이는 마디 수를 더한 값이에요.
 * 실패: 없어요.
 */
    int rowCount(const QModelIndex& parent = {}) const override;
/** 줄의 역할 값을 읽어요.
 * 입력: 줄 index와 Role 값 role이에요.
 * 출력: 역할의 값이에요. 전체 줄은 key "*", branch 거짓, depth 0, expanded 거짓, count는 마지막 setNames의 이름 수예요.
 * 실패: 범위 밖이나 모르는 역할은 빈 값이에요.
 */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
/** 역할 이름을 돌려줘요.
 * 입력: 없어요.
 * 출력: QML에서 쓰는 key·branch·depth·expanded·count예요.
 * 실패: 없어요.
 */
    QHash<int, QByteArray> roleNames() const override;
/** 이름 목록으로 트리를 다시 만들어요.
 * 입력: Params가 드는 순서 그대로의 이름 목록 names예요.
 * 출력: 지금 목록과 같으면 아무것도 하지 않아요(신호도 없어요). 다르면 buildParamTree로 다시 만들고,
 *       없어진 가지의 펼침을 잊은 뒤 modelReset을 내요. 남은 가지의 펼침은 그대로예요.
 * 실패: 없어요.
 */
    void setNames(const QStringList& names);
/** 가지 하나의 펼침을 뒤집어요.
 * 입력: 마디 key예요.
 * 출력: 트리에 있는 가지면 펼침이 바뀌고, 늘거나 준 줄은 rowsInserted·rowsRemoved로, 그 가지 줄은 expanded 역할의
 *       dataChanged로 알려요. 접어도 그 아래 가지들의 펼침은 기억해요.
 * 실패: 가지가 아니면 아무것도 하지 않아요.
 */
    void toggle(const QString& key);
/** 마디가 보이도록 조상 가지를 모두 펼쳐요.
 * 입력: 마디 key예요.
 * 출력: 펼침이 바뀌었으면 modelReset을 내요.
 * 실패: 트리에 없는 key나 첫 단계 마디는 아무것도 하지 않아요.
 */
    void expandTo(const QString& key);
/** 모든 가지를 접어요.
 * 입력: 없어요.
 * 출력: 펼친 가지가 있었으면 펼침을 비우고 modelReset을 내요.
 * 실패: 없어요.
 */
    void collapseAll();
/** 마디가 트리에 있는지 봐요.
 * 입력: 마디 key예요.
 * 출력: "*"이거나 트리에 그 key의 마디가 있으면 참이에요.
 * 실패: 없어요.
 */
    bool contains(const QString& key) const;
/** 가지가 펼쳐져 있는지 봐요.
 * 입력: 마디 key예요.
 * 출력: 펼침에 들어 있으면 참이에요.
 * 실패: 없어요.
 */
    bool isExpanded(const QString& key) const;
private:
    bool isBranch(const QString& key) const;
    int rowOf(const QString& key) const;
    QStringList names_;
    QList<ParamNode> tree_;
    QHash<QString, int> positions_;
    QSet<QString> expanded_;
    QList<ParamNode> rows_;
};
}
