// [0.6.0 5.8 T10] specs/features/0.6.0-fly.md
// [0.5.0 S5 T7] specs/features/0.5.0-map.md
// [0.1.0 T7] specs/features/0.1.0-link.md
// [0.0.3 T2 T15 T21] specs/features/0.0.3-windows.md
// [0.0.1 T1] specs/features/0.0.1-shell.md
// [0.0.4 T2] specs/features/0.0.4-righttabs.md
// [0.0.4 T33 T34] specs/features/0.0.4-righttabs.md
#include <kgc/ui/Slots.h>
#include <kgc/ui/Errors.h>
#include <algorithm>
namespace kgc::ui {
namespace {
// 이름이 wanted에 든 역할 번호만 모아요. 바뀐 역할만 알려 다른 역할(component 등)에 묶인 화면이 다시 읽지 않게 해요.
QList<int> rolesOf(const QHash<int, QByteArray>& names, const QList<QByteArray>& wanted)
{
    QList<int> roles;
    for (auto it = names.cbegin(); it != names.cend(); ++it)
        if (wanted.contains(it.value())) roles.append(it.key());
    return roles;
}
}
TopbarSlot::TopbarSlot(QObject* parent) : SlotModel(parent) {}
kgc::app::errors::Result<void> TopbarSlot::registerPlaceholder(const QString& id, int order, const QString& area,
    int group, const QString& labelKey, const QString& icon, bool iconOnly, const QString& shape)
{
    if (area != u"left" && area != u"centre" && area != u"right") qFatal("registerPlaceholder: unknown topbar area");
    if (shape != u"button" && shape != u"dropdown" && shape != u"display")
        qFatal("registerPlaceholder: unknown topbar shape");
    return add({id, order, {{"area", area}, {"group", group}, {"labelKey", labelKey}, {"icon", icon},
        {"iconOnly", iconOnly}, {"shape", shape}, {"component", QUrl{}}}});
}
kgc::app::errors::Result<void> TopbarSlot::registerItem(const QString& id, int order,
    const QString& area, int group, const QUrl& component)
{
    if (area != u"left" && area != u"centre" && area != u"right") qFatal("registerItem: unknown topbar area");
    return add({id, order, {{"area", area}, {"group", group}, {"component", component}}});
}
kgc::app::errors::Result<void> TopbarSlot::setHidden(const QString& id, bool hidden)
{
    bool before = false;
    for (int row = 0; row < rowCount(); ++row)
        if (entryAt(row).id == id) before = entryAt(row).values.value("hidden").toBool();
    const auto result = setValue(id, QStringLiteral("hidden"), hidden);
    // 숨김이 바뀌면 다른 행의 앞 구분선도 바뀔 수 있어 모든 행의 hidden·separatorBefore만 다시 읽게 해요.
    // 역할을 좁히지 않으면 component에 묶인 상단바 Loader가 다시 계산되어 불러온 항목을 잃어요.
    if (result && before != hidden && rowCount() > 0)
        emit dataChanged(index(0), index(rowCount() - 1), rolesOf(roleNames(), {"hidden", "separatorBefore"}));
    return result;
}
QVariant TopbarSlot::data(const QModelIndex& index, int role) const
{
    const auto name = roleNames().value(role);
    const bool valid = index.isValid() && index.row() >= 0 && index.row() < rowCount();
    if (name == "hidden") return valid ? QVariant(entryAt(index.row()).values.value("hidden").toBool()) : QVariant();
    if (name != "separatorBefore") return SlotModel::data(index, role);
    if (!valid) return false;
    const auto& current = entryAt(index.row());
    if (current.values.value("hidden").toBool()) return false;
    // 숨은 행은 자리가 사라지므로 건너뛰고, 앞쪽으로 보이는 첫 행과 비교해요.
    for (int row = index.row() - 1; row >= 0; --row) {
        const auto& previous = entryAt(row);
        if (previous.values.value("hidden").toBool()) continue;
        return previous.values.value("area") == current.values.value("area")
            && previous.values.value("group") != current.values.value("group");
    }
    return false;
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
    roles.insert(Qt::UserRole + 10, "component");
    roles.insert(Qt::UserRole + 11, "hidden");
    return roles;
}
StatusChipSlot::StatusChipSlot(QObject* parent) : SlotModel(parent) {}
kgc::app::errors::Result<void> StatusChipSlot::registerPlaceholder(const QString& id, int order,
    const QString& side, const QString& labelKey)
{
    if (side != u"left" && side != u"right") qFatal("registerPlaceholder: unknown status chip side");
    return add({id, order, {{"side", side}, {"labelKey", labelKey}, {"component", QUrl{}}}});
}
kgc::app::errors::Result<void> StatusChipSlot::registerItem(const QString& id, int order,
    const QString& side, const QUrl& component)
{
    if (side != u"left" && side != u"right") qFatal("registerItem: unknown status chip side");
    return add({id, order, {{"side", side}, {"component", component}}});
}
QHash<int, QByteArray> StatusChipSlot::roleNames() const
{
    auto roles = SlotModel::roleNames();
    roles.insert(Qt::UserRole + 3, "side");
    roles.insert(Qt::UserRole + 4, "labelKey");
    roles.insert(Qt::UserRole + 5, "component");
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
kgc::app::errors::Result<void> MapLayerSlot::registerLayer(const QString& id, int order, const QUrl& component)
{
    if (component.isEmpty()) qFatal("registerLayer: empty component");
    return add({id, order, {{"component", component}}});
}
QHash<int, QByteArray> MapLayerSlot::roleNames() const
{
    auto roles = SlotModel::roleNames();
    roles.insert(Qt::UserRole + 3, "component");
    return roles;
}
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
    // 끼어든 항목 때문에 뒤 항목의 앞 구분선이 바뀔 수 있어 separatorBefore 역할만 다시 읽게 해요.
    // 바로 뒤 행이 숨었으면 구분선이 바뀌는 행은 더 뒤에 있으므로 끝 행까지 알려요. 그 역할이 없는 슬롯은 알리지 않아요.
    const auto roles = rolesOf(roleNames(), {"separatorBefore"});
    if (row + 1 < entries_.size() && !roles.isEmpty())
        emit dataChanged(index(row + 1), index(int(entries_.size()) - 1), roles);
    return {};
}
kgc::app::errors::Result<void> SlotModel::setValue(const QString& id, const QString& key, const QVariant& value)
{
    for (int row = 0; row < entries_.size(); ++row) {
        auto& entry = entries_[row];
        if (entry.id != id) continue;
        if (entry.values.value(key) == value) return {};
        entry.values.insert(key, value);
        // 바뀐 값의 역할만 알려요. 그 이름의 역할이 없으면 화면이 읽을 값이 아니라 알리지 않아요.
        const auto roles = rolesOf(roleNames(), {key.toLatin1()});
        if (!roles.isEmpty()) emit dataChanged(index(row), index(row), roles);
        return {};
    }
    return std::unexpected(make(UiError::UnknownId));
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
