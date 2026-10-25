// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/app/settings/Settings.h>
#include <QJsonObject>
#include <kgc/app/settings/Range.h>
namespace kgc::app::settings {
Settings::Settings() : storage_(QSettings::IniFormat, QSettings::UserScope,
    "K-GroundControl", "K-GroundControl"), definitions_(registry()) {}
Settings& Settings::instance() { static Settings settings; return settings; }
QJsonArray Settings::definitions() const { return definitions_; }
QVariant Settings::value(const QString& id) const
{
    for (const auto& entry : definitions_) {
        const auto definition = entry.toObject();
        if (definition["id"].toString() != id) continue;
        auto key = id;
        key.replace('.', '/');
        auto result = storage_.value(key, definition["default"].toVariant());
        if (definition["type"] == "enum" && !definition["enum"].toArray().contains(result.toString()))
            result = definition["default"].toVariant();
        if (!withinRange(definition, result)) result = definition["default"].toVariant();
        return result;
    }
    return {};
}
bool Settings::setValue(const QString& id, const QVariant& value)
{
    for (const auto& entry : definitions_) {
        const auto definition = entry.toObject();
        if (definition["id"].toString() != id) continue;
        if (definition["type"] == "enum" && !definition["enum"].toArray().contains(value.toString())) return false;
        if (!withinRange(definition, value)) return false;
        if (this->value(id) == value) return true;
        auto key = id;
        key.replace('.', '/');
        storage_.setValue(key, value);
        storage_.sync();
        emit changed(id);
        return true;
    }
    return false;
}
}
