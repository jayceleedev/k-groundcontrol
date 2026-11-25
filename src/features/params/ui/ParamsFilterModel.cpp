// [0.4.2 5.5] specs/features/0.4.2-param-tree.md
// [0.3.0 6.8] specs/features/0.3.0-params.md
#include "ParamsFilterModel.h"
#include "ParamSearch.h"
#include "ParamTree.h"
#include "ParamValue.h"
namespace kgc::params {
ParamsFilterModel::ParamsFilterModel(QObject* parent) : QSortFilterProxyModel(parent) {}
void ParamsFilterModel::setParams(Params* params)
{
    params_ = params;
    setSourceModel(params);
}
void ParamsFilterModel::setNodeKey(const QString& key)
{
    beginFilterChange();
    nodeKey_ = key;
    endFilterChange(Direction::Rows);
}
void ParamsFilterModel::setTerms(const QStringList& terms)
{
    beginFilterChange();
    terms_ = terms;
    endFilterChange(Direction::Rows);
}
void ParamsFilterModel::setPendingOnly(bool value)
{
    beginFilterChange();
    pendingOnly_ = value;
    endFilterChange(Direction::Rows);
}
void ParamsFilterModel::setNonDefaultOnly(bool value)
{
    beginFilterChange();
    nonDefaultOnly_ = value;
    endFilterChange(Direction::Rows);
}
bool ParamsFilterModel::filterAcceptsRow(int row, const QModelIndex&) const
{
    if (!params_ || row >= params_->all().size()) return false;
    const auto& entry = params_->all()[row];
    if (!underParamNode(entry.name, nodeKey_)) return false;
    if (pendingOnly_ && !entry.pending) return false;
    const auto* meta = params_->meta(entry.name);
    if (nonDefaultOnly_ && params_->hasDefaults()
        && (!meta || !meta->defaultValue || sameValue(*meta->defaultValue, entry.value, entry.type))) return false;
    return matchesSearch(terms_, entry.name, meta ? meta->title : QString{}, meta ? meta->description : QString{});
}
}
