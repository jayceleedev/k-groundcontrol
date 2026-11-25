// [0.4.2 5.2 T1 T2 T3 T4 T5] specs/features/0.4.2-param-tree.md
#include "ParamTree.h"
#include <QHash>
#include <utility>
namespace kgc::params {
namespace {
// 접두어 하나를 몇 개의 이름이 품는지와, 그 가운데 입력에서 가장 먼저 나온 이름의 자리예요.
struct PrefixCount {
    int names = 0;
    int first = -1;
};
// 이름의 접두어를 짧은 것부터 모아요. 빈 조각이 나오면 그 접두어와 더 긴 접두어는 모두 빈 조각을 품으니 멈춰요.
// 맨 앞의 "_"도 빈 조각이라 그런 이름에는 접두어가 없어요(Mission Planner와 같아요).
QStringList prefixesOf(const QString& name)
{
    QStringList result;
    qsizetype start = 0;
    for (auto end = name.indexOf(u'_'); end >= 0; end = name.indexOf(u'_', end + 1)) {
        if (end == start) break;
        result.append(name.left(end));
        start = end + 1;
    }
    return result;
}
QString branchKey(const QString& prefix) { return prefix + QStringLiteral("_*"); }
// 부모 접두어(첫 단계는 빈 문자열)마다 자식 마디를 첫 이름 순서로 들고 있어요.
using Children = QHash<QString, QList<ParamNode>>;
void appendSubtree(const Children& children, const QString& parent, int depth, QList<ParamNode>& out)
{
    const auto found = children.constFind(parent);
    if (found == children.cend()) return;
    for (auto node : found.value()) {
        node.depth = depth;
        out.append(node);
        if (node.branch) appendSubtree(children, paramNodeText(node.key), depth + 1, out);
    }
}
}
QList<ParamNode> buildParamTree(const QStringList& names)
{
    // 이름마다 접두어를 한 번씩 세요. 가지마다 전체 이름을 다시 훑지 않아요.
    QHash<QString, PrefixCount> counts;
    QList<QStringList> prefixes;
    prefixes.reserve(names.size());
    for (int index = 0; index < names.size(); ++index) {
        auto own = prefixesOf(names[index]);
        for (const auto& prefix : own) {
            auto& entry = counts[prefix];
            if (entry.names++ == 0) entry.first = index;
        }
        prefixes.append(std::move(own));
    }
    const QSet<QString> nameSet(names.cbegin(), names.cend());
    // 이름을 입력 순서로 지나며 마디를 부모 아래에 붙이면, 형제는 저절로 첫 이름 순서가 돼요.
    // 가지는 그 가지의 첫 이름을 지날 때 붙이고, 부모는 같은 이름의 더 짧은 가지 접두어 가운데 가장 긴 것이에요.
    Children children;
    for (int index = 0; index < names.size(); ++index) {
        QString parent;
        for (const auto& prefix : prefixes[index]) {
            const auto entry = counts.value(prefix);
            if (entry.names < 2) continue;
            if (entry.first == index) {
                const int count = entry.names + (nameSet.contains(prefix) ? 1 : 0);
                children[parent].append(ParamNode{branchKey(prefix), true, 0, count});
            }
            parent = prefix;
        }
        children[parent].append(ParamNode{names[index], false, 0, 1});
    }
    QList<ParamNode> tree;
    tree.reserve(names.size() + counts.size());
    appendSubtree(children, QString{}, 0, tree);
    return tree;
}
bool underParamNode(const QString& name, const QString& key)
{
    if (!key.endsWith(u'*')) return name == key;
    const auto pattern = QStringView(key).chopped(1);
    if (name.startsWith(pattern)) return true;
    // 이름이 접두어와 똑같은 파라미터(TUNE과 가지 TUNE_*)도 그 가지에 들어가요.
    return pattern.size() == name.size() + 1 && pattern.endsWith(u'_') && pattern.startsWith(name);
}
QList<ParamNode> visibleParamNodes(const QList<ParamNode>& tree, const QSet<QString>& expanded)
{
    // open[d]는 깊이 d의 가장 최근 가지 아래가 보이는지예요. 앞선 순회라 부모는 언제나 그 깊이의 가장 최근 가지예요.
    QList<bool> open;
    QList<ParamNode> result;
    for (const auto& node : tree) {
        const bool visible = node.depth == 0 || (node.depth <= open.size() && open[node.depth - 1]);
        if (node.branch) {
            open.resize(node.depth + 1);
            open[node.depth] = visible && expanded.contains(node.key);
        }
        if (visible) result.append(node);
    }
    return result;
}
QString paramNodeText(const QString& key)
{
    return key.endsWith(QStringLiteral("_*")) ? key.chopped(2) : key;
}
}
