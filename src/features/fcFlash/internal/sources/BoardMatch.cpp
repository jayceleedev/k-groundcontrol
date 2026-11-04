// [0.4.1 5.3 T1 T2] specs/features/0.4.1-board-match.md
#include "BoardMatch.h"
#include "Candidates.h"
#include <algorithm>
namespace kgc::fcFlash {
bool nameMatches(const QString& usbName, const QStringList& names)
{
    const auto name = usbName.trimmed();
    if (name.isEmpty()) return false;
    return std::any_of(names.cbegin(), names.cend(), [&name](const QString& value) {
        return value.trimmed().compare(name, Qt::CaseInsensitive) == 0;
    });
}
MatchResult recognize(const QString& usbName, const QList<QStringList>& candidateNames)
{
    if (usbName.trimmed().isEmpty()) return {BoardMatch::NoName, {}};
    MatchResult result{BoardMatch::NoMatch, {}};
    for (int i = 0; i < candidateNames.size(); ++i)
        if (nameMatches(usbName, candidateNames.at(i))) result.matched.append(i);
    if (!result.matched.isEmpty()) result.kind = BoardMatch::Matched;
    return result;
}
int preselect(const MatchResult& result, int count)
{
    if (result.kind == BoardMatch::Matched) return result.matched.size() == 1 ? result.matched.first() : -1;
    if (result.kind == BoardMatch::NoName) return pickCandidate(count) == CandidatePick::Auto ? 0 : -1;
    return -1;
}
}
