// [0.0.3 T2 T15 T21] specs/features/0.0.3-windows.md
// [0.0.1 T1] specs/features/0.0.1-shell.md
// [0.0.4 T2] specs/features/0.0.4-righttabs.md
// [0.0.4 T33 T34] specs/features/0.0.4-righttabs.md
#include <kgc/ui/Slots.h>
#include <kgc/ui/Errors.h>
#include <algorithm>
namespace kgc::ui {
TopbarSlot::TopbarSlot(QObject* parent) : SlotModel(parent) {}
kgc::app::errors::Result<void> TopbarSlot::registerPlaceholder(const QString& id, int order, const QString& area,
    int group, const QString& labelKey, const QString& icon, bool iconOnly, const QString& shape)
{
    if (area != u"left" && area != u"centre" && area != u"right") qFatal("registerPlaceholder: unknown topbar area");
    if (shape != u"button" && shape != u"dropdown" && shape != u"display")
        qFatal("registerPlaceholder: unknown topbar shape");
    return add({id, order, {{"area", area}, {"group", group}, {"labelKey", labelKey}, {"icon", icon},
        {"iconOnly", iconOnly}, {"shape", shape}}});
}
QVariant TopbarSlot::data(const QModelIndex& index, int role) const
{
    if (roleNames().value(role) != "separatorBefore") return SlotModel::data(index, role);
    if (!index.isValid() || index.row() <= 0 || index.row() >= rowCount()) return false;
    const auto& current = entryAt(index.row());
    const auto& previous = entryAt(index.row() - 1);
    return previous.values.value("area") == current.values.value("area")
        && previous.values.value("group") != current.values.value("group");
}
QHash<int, QByteArray> TopbarSlot::roleNames() const
{
    auto roles = SlotModel::roleNames();
    roles.insert(Qt::UserRole + 3, "area");
    roles.insert(Qt::UserRole + 4, "group");
    roles.insert(Qt::UserRole + 5, "labelKey");
    roles.insert(Qt::UserRole + 6, "icon");
    roles.insert(Qt::UserRole + 7, "iconOnly");
    roles.insert(Qt::UserRole + 8, "separatorBefore");
    roles.insert(Qt::UserRole + 9, "shape");
    return roles;
}
StatusChipSlot::StatusChipSlot(QObject* parent) : SlotModel(parent) {}
kgc::app::errors::Result<void> StatusChipSlot::registerPlaceholder(const QString& id, int order,
    const QString& side, const QString& labelKey)
{
    if (side != u"left" && side != u"right") qFatal("registerPlaceholder: unknown status chip side");
    return add({id, order, {{"side", side}, {"labelKey", labelKey}}});
}
QHash<int, QByteArray> StatusChipSlot::roleNames() const
{
    auto roles = SlotModel::roleNames();
    roles.insert(Qt::UserRole + 3, "side");
    roles.insert(Qt::UserRole + 4, "labelKey");
    return roles;
}
RightTabSlot::RightTabSlot(QObject* parent) : SlotModel(parent) {}
kgc::app::errors::Result<void> RightTabSlot::registerTab(const QString& id, int order, const QString& titleKey,
    const QUrl& component, RightTabPlacement placement)
{
    const bool top = placement == RightTabPlacement::Top;
    // 같은 id는 Top 중복보다 먼저 알려야 해서(스펙의 검사 순서) add에 맡기기 전에 직접 봐요.
    for (int row = 0; row < rowCount(); ++row) {
        if (entryAt(row).id == id) return std::unexpected(make(UiError::DuplicateId));
    }
    for (int row = 0; top && row < rowCount(); ++row) {
        if (entryAt(row).values.value("placement").toString() == u"top") return std::unexpected(make(UiError::TopTaken));
    }
    return add({id, order, {{"titleKey", titleKey}, {"component", component},
        {"placement", top ? QStringLiteral("top") : QStringLiteral("tab")}}});
}
QHash<int, QByteArray> RightTabSlot::roleNames() const
{
    auto roles = SlotModel::roleNames();
    roles.insert(Qt::UserRole + 3, "titleKey");
    roles.insert(Qt::UserRole + 4, "component");
    roles.insert(Qt::UserRole + 5, "placement");
    return roles;
}
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
    // 끼어든 항목 때문에 바로 뒤 항목의 앞 구분선 같은 이웃 기준 값이 바뀔 수 있어 다시 읽게 해요.
    if (row + 1 < entries_.size()) emit dataChanged(index(row + 1), index(row + 1));
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
    // 파생 슬롯까지 정해진 뒤 처음 읽을 때만 역할 표를 만들어 매 행에서 재생성하지 않아요.
    if (roles_.isEmpty()) roles_ = roleNames();
    const auto name = roles_.value(role);
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
    // 이미 등록된 동작은 대체 요청의 호출 대상과 무관하게 그대로 보존해요.
    if (actions_.contains(id)) return std::unexpected(make(UiError::DuplicateId));
    const auto signature = QMetaObject::normalizedSignature((QByteArray(method) + "()").constData());
    if (!receiver || !method || receiver->metaObject()->indexOfMethod(signature.constData()) < 0)
        qFatal("registerAction: receiver or method missing");
    const auto result = add({id, order, {{"icon", icon}, {"hintKey", hintKey}}});
    if (result) actions_.insert(id, {receiver, signature.chopped(2)});
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
