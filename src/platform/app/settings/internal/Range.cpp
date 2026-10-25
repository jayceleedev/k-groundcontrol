// [0.0.2 T20] specs/features/0.0.2-theme.md
#include <kgc/app/settings/Range.h>
#include <cmath>
namespace kgc::app::settings {
bool withinRange(const QJsonObject& definition, const QVariant& value)
{
    const auto type = definition["type"].toString();
    if (type != "int" && type != "double") return true;
    bool converted = false;
    const double number = value.toDouble(&converted);
    if (!converted || !std::isfinite(number)) return false;
    return (!definition.contains("min") || number >= definition["min"].toDouble())
        && (!definition.contains("max") || number <= definition["max"].toDouble());
}
}
