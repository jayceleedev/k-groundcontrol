// [0.3.0 5.5 T7] specs/features/0.3.0-params.md
#include "ParamGrouping.h"
#include <QHash>
namespace kgc::params {
QStringList mergeSingleGroups(const QStringList& groups)
{
    QHash<QString, int> counts;
    for (const auto& group : groups) ++counts[group];
    auto result = groups;
    for (auto& group : result)
        if (counts.value(group) <= 1) group.clear();
    return result;
}
}
