// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/ui/PanelOrder.h>
#include <kgc/ui/Errors.h>
#include <algorithm>
namespace kgc::ui {
std::vector<PanelEntry> sortByOrder(std::vector<PanelEntry> entries)
{
    std::stable_sort(entries.begin(), entries.end(), [](const auto& left, const auto& right) {
        return left.order < right.order;
    });
    return entries;
}
std::vector<bool> separatorAfter(const std::vector<PanelEntry>& sorted)
{
    std::vector<bool> result(sorted.size(), false);
    for (std::size_t index = 0; index + 1 < sorted.size(); ++index)
        result[index] = sorted[index].group != sorted[index + 1].group;
    return result;
}
kgc::app::errors::Result<void> checkDuplicate(const std::vector<PanelEntry>& entries, const QString& id)
{
    if (std::ranges::any_of(entries, [&id](const auto& entry) { return entry.id == id; }))
        return std::unexpected(make(UiError::DuplicateId));
    return {};
}
}
