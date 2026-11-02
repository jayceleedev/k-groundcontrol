// [0.3.0 5.5 6.3 6.4 6.8-1] specs/features/0.3.0-params.md
#include <kgc/params/Params.h>
#include "ParamSearch.h"
#include "ParamValue.h"
#include <QCoreApplication>
#include <QEvent>
#include <algorithm>
namespace kgc::params {
namespace {
QString kindOf(const ParamMeta* meta)
{
    if (meta && !meta->bits.isEmpty()) return QStringLiteral("bitmask");
    if (meta && !meta->options.isEmpty()) return QStringLiteral("option");
    return QStringLiteral("number");
}
QString optionLabel(const ParamMeta* meta, double value, std::uint8_t type)
{
    if (!meta) return {};
    for (const auto& option : meta->options)
        if (sameValue(option.value, value, type)) return option.label;
    return {};
}
QString rangeText(const Param& row, const ParamMeta* meta)
{
    if (!meta) return {};
    const auto kind = kindOf(meta);
    if (kind == QLatin1String("bitmask")) {
        const char* key = QT_TRANSLATE_NOOP("Params", "비트 %1개");
        return QCoreApplication::translate("Params", key).arg(meta->bits.size());
    }
    if (kind == QLatin1String("number")) return formatRange(*meta, row.type);
    QStringList parts;
    for (const auto& option : meta->options)
        parts.append(QStringLiteral("%1 %2").arg(formatValue(option.value, row.type), option.label));
    return parts.join(QStringLiteral(", "));
}
QVariant defaultRole(const Param& row, const ParamMeta* meta, int role)
{
    const bool known = meta && meta->defaultValue;
    switch (role) {
    case Params::HasDefaultRole: return known;
    case Params::IsDefaultRole: return known && sameValue(*meta->defaultValue, row.value, row.type);
    case Params::DefaultTextRole: {
        if (!known) return QString{};
        const auto label = optionLabel(meta, *meta->defaultValue, row.type);
        return label.isEmpty() ? formatValue(*meta->defaultValue, row.type) : label;
    }
    default: return {};
    }
}
QVariant metaRole(const Param& row, const ParamMeta* meta, int role)
{
    switch (role) {
    case Params::UnitsRole: return meta ? meta->units : QString{};
    case Params::RangeTextRole: return rangeText(row, meta);
    case Params::OptionTextRole: return optionLabel(meta, row.pending.value_or(row.value), row.type);
    case Params::TitleRole: return meta ? meta->title : QString{};
    case Params::DescriptionRole: return meta ? meta->description : QString{};
    case Params::ReadOnlyRole: return meta && meta->readOnly;
    case Params::RebootRequiredRole: return meta && meta->rebootRequired;
    case Params::KindRole: return kindOf(meta);
    default: return defaultRole(row, meta, role);
    }
}
}
Params::Params(QObject* parent) : QAbstractListModel(parent)
{
    // 언어가 바뀌면 Translator가 번역기를 갈아 끼우고, 그때 앱에 오는 사건으로 알 수 있어요.
    if (qApp) qApp->installEventFilter(this);
}
int Params::rowCount(const QModelIndex& parent) const { return parent.isValid() ? 0 : rows_.size(); }
QVariant Params::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= rows_.size()) return {};
    const auto& row = rows_[index.row()];
    switch (role) {
    case NameRole: return row.name;
    case ValueRole: return row.value;
    case ValueTextRole: return formatValue(row.value, row.type);
    case TypeRole: return row.type;
    case PendingRole: return row.pending ? QVariant(*row.pending) : QVariant{};
    case PendingTextRole: return row.pending ? formatValue(*row.pending, row.type) : QString{};
    case HasPendingRole: return row.pending.has_value();
    case GroupRole: return row.group;
    default: return metaRole(row, meta(row.name), role);
    }
}
QHash<int, QByteArray> Params::roleNames() const
{
    return {{NameRole, "name"}, {ValueRole, "value"}, {ValueTextRole, "valueText"}, {TypeRole, "type"},
            {PendingRole, "pending"}, {PendingTextRole, "pendingText"}, {HasPendingRole, "hasPending"},
            {IsDefaultRole, "isDefault"}, {HasDefaultRole, "hasDefault"}, {DefaultTextRole, "defaultText"},
            {UnitsRole, "units"}, {RangeTextRole, "rangeText"}, {OptionTextRole, "optionText"}, {TitleRole, "title"},
            {DescriptionRole, "description"}, {GroupRole, "group"}, {ReadOnlyRole, "readOnly"},
            {RebootRequiredRole, "rebootRequired"}, {KindRole, "kind"}};
}
int Params::lowerBound(const QString& name) const
{
    const auto found = std::lower_bound(rows_.cbegin(), rows_.cend(), name,
        [](const Param& row, const QString& key) { return naturalLess(row.name, key); });
    return static_cast<int>(found - rows_.cbegin());
}
int Params::rowOf(const QString& name) const
{
    const int row = lowerBound(name);
    return row < rows_.size() && rows_[row].name == name ? row : -1;
}
const Param* Params::find(const QString& name) const
{
    const int row = rowOf(name);
    return row < 0 ? nullptr : &rows_[row];
}
const ParamMeta* Params::meta(const QString& name) const
{
    const auto found = metadata_.constFind(name);
    return found == metadata_.cend() ? nullptr : &found.value();
}
const QList<Param>& Params::all() const { return rows_; }
int Params::pendingCount() const { return pendingCount_; }
bool Params::hasDefaults() const { return hasDefaults_; }
bool Params::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == qApp && event->type() == QEvent::LanguageChange && !rows_.isEmpty())
        emit dataChanged(index(0), index(rows_.size() - 1), {RangeTextRole});
    return false;
}
void Params::touch(int row) { emit dataChanged(index(row), index(row)); }
bool Params::store(const QString& name, double value, std::uint8_t type, const QString& group)
{
    const int position = lowerBound(name);
    if (position < rows_.size() && rows_[position].name == name) {
        auto& row = rows_[position];
        row.value = value;
        row.type = type;
        const bool settled = row.pending && sameValue(*row.pending, value, type);
        if (settled) {
            row.pending.reset();
            --pendingCount_;
        }
        touch(position);
        if (settled) emit pendingCountChanged();
        return false;
    }
    beginInsertRows({}, position, position);
    rows_.insert(position, Param{name, value, type, std::nullopt, group});
    endInsertRows();
    return true;
}
void Params::setPending(const QString& name, std::optional<double> pending)
{
    const int position = rowOf(name);
    if (position < 0) return;
    auto& row = rows_[position];
    if (row.pending == pending) return;
    pendingCount_ += static_cast<int>(pending.has_value()) - static_cast<int>(row.pending.has_value());
    const bool changed = pending.has_value() != row.pending.has_value();
    row.pending = pending;
    touch(position);
    if (changed) emit pendingCountChanged();
}
void Params::setMetadata(QHash<QString, ParamMeta> metadata)
{
    metadata_ = std::move(metadata);
    hasDefaults_ = std::any_of(metadata_.cbegin(), metadata_.cend(), [](const ParamMeta& meta) { return meta.defaultValue.has_value(); });
    if (!rows_.isEmpty()) emit dataChanged(index(0), index(rows_.size() - 1));
}
void Params::setGroups(const QStringList& groups)
{
    Q_ASSERT(groups.size() == rows_.size());
    for (qsizetype i = 0; i < rows_.size(); ++i) rows_[i].group = groups[i];
    if (!rows_.isEmpty()) emit dataChanged(index(0), index(rows_.size() - 1), {GroupRole});
    emit groupsChanged();
}
void Params::removeExcept(const QSet<QString>& names)
{
    const int before = pendingCount_;
    for (int row = rows_.size() - 1; row >= 0; --row) {
        if (names.contains(rows_[row].name)) continue;
        beginRemoveRows({}, row, row);
        pendingCount_ -= static_cast<int>(rows_[row].pending.has_value());
        rows_.removeAt(row);
        endRemoveRows();
    }
    if (pendingCount_ != before) emit pendingCountChanged();
}
}
