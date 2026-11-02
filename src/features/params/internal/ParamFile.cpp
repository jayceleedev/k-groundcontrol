// [0.3.0 5.5 6.8-1 T3] specs/formats/params.md
#include "ParamFile.h"
#include "ParamSearch.h"
#include "ParamValue.h"
#include <kgc/params/Errors.h>
#include <QHash>
#include <QRegularExpression>
#include <algorithm>
#include <cmath>
#include <optional>
namespace kgc::params {
namespace {
constexpr int kQgcCells = 5;
struct FileLine { QString name; QString value; bool otherComponent = false; };
// 다섯 칸의 QGC 줄이면 이름과 값을, 아니면 쉼표·공백으로 나눈 앞 두 칸을 돌려줘요.
std::optional<FileLine> splitLine(const QString& line)
{
    const auto cells = line.split(QLatin1Char('\t'));
    if (cells.size() == kQgcCells) {
        bool vehicle = false;
        bool component = false;
        bool type = false;
        cells[0].toInt(&vehicle);
        const int id = cells[1].toInt(&component);
        cells[4].toInt(&type);
        if (vehicle && component && type) return FileLine{cells[2], cells[3], id != 1};
    }
    static const QRegularExpression separator(QStringLiteral("[,\\s]+"));
    const auto parts = line.split(separator);
    if (parts.size() < 2) return std::nullopt;
    return FileLine{parts[0], parts[1], false};
}
}
QByteArray writeParamFile(const ParamFileHeader& header, const QList<Param>& rows)
{
    auto sorted = rows;
    std::sort(sorted.begin(), sorted.end(), [](const Param& a, const Param& b) { return naturalLess(a.name, b.name); });
    QString text = QStringLiteral("# Onboard parameters for Vehicle %1\n#\n# Stack: %2\n# Vehicle: %3\n# Version: %4\n#\n"
                                  "# Vehicle-Id\tComponent-Id\tName\tValue\tType\n")
                       .arg(header.sysid).arg(header.firmwareName).arg(header.vehicleType).arg(header.firmwareVersion);
    for (const auto& row : sorted)
        text += QStringLiteral("%1\t1\t%2\t%3\t%4\n").arg(header.sysid).arg(row.name, formatValue(row.value, row.type)).arg(row.type);
    return text.toUtf8();
}
kgc::app::errors::Result<ParsedParamFile> parseParamFile(const QByteArray& bytes)
{
    ParsedParamFile parsed;
    QHash<QString, qsizetype> seen;
    const auto lines = QString::fromUtf8(bytes).split(QLatin1Char('\n'));
    for (const auto& raw : lines) {
        const auto line = raw.trimmed();
        if (line.isEmpty() || line.startsWith(QLatin1Char('#'))) continue;
        const auto cells = splitLine(line);
        if (!cells) { ++parsed.badLines; continue; }
        if (cells->otherComponent) { ++parsed.otherComponent; continue; }
        bool ok = false;
        const double value = cells->value.toDouble(&ok);
        if (!ok || !std::isfinite(value) || cells->name.isEmpty()) { ++parsed.badLines; continue; }
        if (const auto found = seen.constFind(cells->name); found != seen.cend()) {
            parsed.entries[*found].second = value;
        } else {
            seen.insert(cells->name, parsed.entries.size());
            parsed.entries.append({cells->name, value});
        }
    }
    if (parsed.entries.isEmpty()) return std::unexpected(make(ParamError::FileEmpty));
    return parsed;
}
}
