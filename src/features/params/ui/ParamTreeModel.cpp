// [0.4.2 5.4] specs/features/0.4.2-param-tree.md
#include "ParamTreeModel.h"
#include <utility>
namespace kgc::params {
ParamTreeModel::ParamTreeModel(QObject* parent) : QAbstractListModel(parent) {}
int ParamTreeModel::rowCount(const QModelIndex& parent) const { return parent.isValid() ? 0 : rows_.size() + 1; }
QVariant ParamTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() > rows_.size()) return {};
    // 0번은 모든 마디를 품는 뿌리가 아니라 첫 단계와 나란한 전체 줄이에요.
    const auto node = index.row() == 0 ? ParamNode{QStringLiteral("*"), false, 0, static_cast<int>(names_.size())}
                                       : rows_[index.row() - 1];
    switch (role) {
    case KeyRole: return node.key;
    case BranchRole: return node.branch;
    case DepthRole: return node.depth;
    case ExpandedRole: return node.branch && expanded_.contains(node.key);
    case CountRole: return node.count;
    default: return {};
    }
}
QHash<int, QByteArray> ParamTreeModel::roleNames() const
{
    return {{KeyRole, "key"}, {BranchRole, "branch"}, {DepthRole, "depth"}, {ExpandedRole, "expanded"}, {CountRole, "count"}};
}
bool ParamTreeModel::isBranch(const QString& key) const
{
    const auto found = positions_.constFind(key);
    return found != positions_.cend() && tree_[found.value()].branch;
}
int ParamTreeModel::rowOf(const QString& key) const
{
    for (int row = 0; row < rows_.size(); ++row)
        if (rows_[row].key == key) return row;
    return -1;
}
void ParamTreeModel::setNames(const QStringList& names)
{
    if (names == names_) return;
    beginResetModel();
    names_ = names;
    tree_ = buildParamTree(names);
    positions_.clear();
    for (int position = 0; position < tree_.size(); ++position) positions_.insert(tree_[position].key, position);
    expanded_.removeIf([this](const QString& key) { return !isBranch(key); });
    rows_ = visibleParamNodes(tree_, expanded_);
    endResetModel();
}
void ParamTreeModel::toggle(const QString& key)
{
    if (!isBranch(key)) return;
    const bool expanding = !expanded_.contains(key);
    auto expanded = expanded_;
    if (expanding) expanded.insert(key);
    else expanded.remove(key);
    auto next = visibleParamNodes(tree_, expanded);
    const int row = rowOf(key);
    // 접힌 가지 아래의 가지면 보이는 줄은 그대로라 펼침만 바꿔요.
    if (row < 0) {
        expanded_ = std::move(expanded);
        return;
    }
    // 바뀌는 줄은 그 가지의 자손이라 가지 줄 바로 뒤에 이어져 있어요. 0번 전체 줄만큼 한 칸 밀려요.
    const int first = row + 2;
    const int changed = static_cast<int>(expanding ? next.size() - rows_.size() : rows_.size() - next.size());
    if (changed > 0) {
        if (expanding) beginInsertRows({}, first, first + changed - 1);
        else beginRemoveRows({}, first, first + changed - 1);
    }
    expanded_ = std::move(expanded);
    rows_ = std::move(next);
    if (changed > 0) {
        if (expanding) endInsertRows();
        else endRemoveRows();
    }
    emit dataChanged(index(row + 1), index(row + 1), {ExpandedRole});
}
void ParamTreeModel::expandTo(const QString& key)
{
    const auto found = positions_.constFind(key);
    if (found == positions_.cend()) return;
    auto expanded = expanded_;
    int depth = tree_[found.value()].depth;
    // 앞선 순회라 앞쪽으로 가며 처음 만나는 한 단계 얕은 마디가 부모 가지예요.
    for (int position = found.value() - 1; position >= 0 && depth > 0; --position) {
        if (tree_[position].depth != depth - 1) continue;
        expanded.insert(tree_[position].key);
        --depth;
    }
    if (expanded == expanded_) return;
    beginResetModel();
    expanded_ = std::move(expanded);
    rows_ = visibleParamNodes(tree_, expanded_);
    endResetModel();
}
void ParamTreeModel::collapseAll()
{
    if (expanded_.isEmpty()) return;
    beginResetModel();
    expanded_.clear();
    rows_ = visibleParamNodes(tree_, expanded_);
    endResetModel();
}
bool ParamTreeModel::contains(const QString& key) const { return key == QStringLiteral("*") || positions_.contains(key); }
bool ParamTreeModel::isExpanded(const QString& key) const { return expanded_.contains(key); }
}
