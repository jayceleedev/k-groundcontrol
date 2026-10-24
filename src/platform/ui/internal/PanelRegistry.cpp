// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/ui/PanelRegistry.h>
namespace kgc::ui {
PanelRegistry::PanelRegistry(QObject* parent) : QAbstractListModel(parent) {}
kgc::app::errors::Result<void> PanelRegistry::registerPanel(PanelEntry entry)
{
    auto result = checkDuplicate(entries_, entry.id);
    if (!result) return result;
    beginResetModel();
    entries_.push_back(std::move(entry));
    entries_ = sortByOrder(std::move(entries_));
    separators_ = separatorAfter(entries_);
    endResetModel();
    return {};
}
kgc::app::errors::Result<void> PanelRegistry::registerPlaceholder(
    const QString& id, int order, int group, const QString& titleKey, const QUrl& icon)
{
    return registerPanel({id, order, group, titleKey, icon, {}});
}
int PanelRegistry::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(entries_.size());
}
QVariant PanelRegistry::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) return {};
    const auto& entry = entries_[index.row()];
    switch (static_cast<Role>(role)) {
    case Role::Id: return entry.id;
    case Role::Order: return entry.order;
    case Role::Group: return entry.group;
    case Role::TitleKey: return entry.titleKey;
    case Role::Icon: return entry.icon;
    case Role::Component: return entry.component;
    case Role::SeparatorAfter: return static_cast<bool>(separators_[index.row()]);
    }
    return {};
}
QHash<int, QByteArray> PanelRegistry::roleNames() const
{
    return {{int(Role::Id), "id"}, {int(Role::Order), "order"}, {int(Role::Group), "group"},
            {int(Role::TitleKey), "titleKey"}, {int(Role::Icon), "icon"},
            {int(Role::Component), "component"}, {int(Role::SeparatorAfter), "separatorAfter"}};
}
}
