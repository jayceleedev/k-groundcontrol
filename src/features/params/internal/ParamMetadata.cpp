// [0.3.0 6.3 T4] specs/features/0.3.0-params.md
#include "ParamMetadata.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <algorithm>
#include <optional>
namespace kgc::params {
namespace {
std::optional<double> number(const QJsonValue& value)
{
    if (value.isDouble()) return value.toDouble();
    bool ok = false;
    const double parsed = value.toString().toDouble(&ok);
    return ok ? std::optional(parsed) : std::nullopt;
}
ParamMeta flatEntry(const QJsonObject& entry)
{
    ParamMeta meta;
    meta.name = entry[QStringLiteral("name")].toString();
    meta.title = entry[QStringLiteral("shortDesc")].toString();
    meta.description = entry[QStringLiteral("longDesc")].toString();
    meta.units = entry[QStringLiteral("units")].toString();
    meta.group = entry[QStringLiteral("group")].toString();
    meta.min = number(entry[QStringLiteral("min")]);
    meta.max = number(entry[QStringLiteral("max")]);
    meta.defaultValue = number(entry[QStringLiteral("default")]);
    meta.rebootRequired = entry[QStringLiteral("rebootRequired")].toBool();
    for (const auto value : entry[QStringLiteral("values")].toArray()) {
        const auto option = value.toObject();
        if (const auto parsed = number(option[QStringLiteral("value")]))
            meta.options.append({*parsed, option[QStringLiteral("description")].toString()});
    }
    for (const auto value : entry[QStringLiteral("bitmask")].toArray()) {
        const auto bit = value.toObject();
        if (const auto parsed = number(bit[QStringLiteral("index")]))
            meta.bits.append({static_cast<int>(*parsed), bit[QStringLiteral("description")].toString()});
    }
    return meta;
}
bool flag(const QJsonObject& entry, const QString& key) { return entry[key].toString() == QStringLiteral("True"); }
ParamMeta groupedEntry(const QString& group, const QString& name, const QJsonObject& entry)
{
    ParamMeta meta;
    meta.name = name;
    meta.group = group;
    meta.title = entry[QStringLiteral("DisplayName")].toString();
    meta.description = entry[QStringLiteral("Description")].toString();
    meta.units = entry[QStringLiteral("Units")].toString();
    const auto range = entry[QStringLiteral("Range")].toObject();
    meta.min = number(range[QStringLiteral("low")]);
    meta.max = number(range[QStringLiteral("high")]);
    meta.rebootRequired = flag(entry, QStringLiteral("RebootRequired"));
    meta.readOnly = flag(entry, QStringLiteral("ReadOnly"));
    const auto values = entry[QStringLiteral("Values")].toObject();
    for (auto it = values.begin(); it != values.end(); ++it)
        if (const auto parsed = number(it.key())) meta.options.append({*parsed, it.value().toString()});
    std::sort(meta.options.begin(), meta.options.end(), [](const auto& a, const auto& b) { return a.value < b.value; });
    const auto bits = entry[QStringLiteral("Bitmask")].toObject();
    for (auto it = bits.begin(); it != bits.end(); ++it)
        if (const auto parsed = number(it.key())) meta.bits.append({static_cast<int>(*parsed), it.value().toString()});
    std::sort(meta.bits.begin(), meta.bits.end(), [](const auto& a, const auto& b) { return a.bit < b.bit; });
    return meta;
}
}
QHash<QString, ParamMeta> parseFlatMetadata(const QByteArray& bytes)
{
    QHash<QString, ParamMeta> result;
    for (const auto value : QJsonDocument::fromJson(bytes).object()[QStringLiteral("parameters")].toArray()) {
        auto meta = flatEntry(value.toObject());
        if (!meta.name.isEmpty()) result.insert(meta.name, std::move(meta));
    }
    return result;
}
QHash<QString, ParamMeta> parseGroupedMetadata(const QByteArray& bytes)
{
    QHash<QString, ParamMeta> result;
    const auto root = QJsonDocument::fromJson(bytes).object();
    for (auto group = root.begin(); group != root.end(); ++group) {
        if (group.key() == QStringLiteral("json")) continue;
        const auto entries = group.value().toObject();
        for (auto entry = entries.begin(); entry != entries.end(); ++entry)
            result.insert(entry.key(), groupedEntry(group.key(), entry.key(), entry.value().toObject()));
    }
    return result;
}
}
