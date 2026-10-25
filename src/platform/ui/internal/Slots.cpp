// [0.0.3 T2 T15] specs/features/0.0.3-windows.md
// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/ui/Slots.h>
#include <kgc/ui/Errors.h>
#include <algorithm>
namespace kgc::ui {
TopbarSlot::TopbarSlot(QObject* parent) : SlotModel(parent) {}
StatusChipSlot::StatusChipSlot(QObject* parent) : SlotModel(parent) {}
RightTabSlot::RightTabSlot(QObject* parent) : SlotModel(parent) {}
MapLayerSlot::MapLayerSlot(QObject* parent) : SlotModel(parent) {}
kgc::app::errors::Result<void> SlotModel::add(SlotEntry entry)
{
    for (const auto& current : entries_)
        if (current.id == entry.id) return std::unexpected(make(UiError::DuplicateId));
    const auto it = std::lower_bound(entries_.cbegin(), entries_.cend(), entry,
        [](const SlotEntry& a, const SlotEntry& b) {
            return a.order == b.order ? a.id < b.id : a.order < b.order;
        });
    const int row = int(it - entries_.cbegin());
    beginInsertRows({}, row, row);
    entries_.insert(row, std::move(entry));
    endInsertRows();
    return {};
}
int SlotModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : int(entries_.size());
}
QVariant SlotModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= entries_.size()) return {};
    const auto& entry = entries_[index.row()];
    const auto name = roleNames().value(role);
    if (name == "id") return entry.id;
    if (name == "order") return entry.order;
    return entry.values.value(QString::fromLatin1(name));
}
QHash<int, QByteArray> SlotModel::roleNames() const
{
    return {{Qt::UserRole + 1, "id"}, {Qt::UserRole + 2, "order"}};
}
BottomActionSlot::BottomActionSlot(QObject* parent) : SlotModel(parent) {}
kgc::app::errors::Result<void> BottomActionSlot::registerAction(const QString& id, int order,
    const QString& icon, const QString& hintKey, QObject* receiver, const char* method)
{
    const auto result = add({id, order, {{"icon", icon}, {"hintKey", hintKey}}});
    if (result) actions_.insert(id, {receiver, method});
    return result;
}
void BottomActionSlot::trigger(const QString& id)
{
    const auto action = actions_.value(id);
    if (action.first) QMetaObject::invokeMethod(action.first, action.second.constData());
}
QHash<int, QByteArray> BottomActionSlot::roleNames() const
{
    auto roles = SlotModel::roleNames();
    roles.insert(Qt::UserRole + 3, "icon");
    roles.insert(Qt::UserRole + 4, "hintKey");
    return roles;
}
}
