// [0.5.0 5.6.3 T3] specs/features/0.5.0-map.md
#include "SizeText.h"
#include "Constants.h"
namespace kgc::map {
QString sizeText(qint64 bytes)
{
    const qint64 megabyte = kBytesPerKilobyte * kBytesPerKilobyte;
    const qint64 gigabyte = megabyte * kBytesPerKilobyte;
    const qint64 unit = bytes < megabyte ? kBytesPerKilobyte : bytes < gigabyte ? megabyte : gigabyte;
    const auto name = unit == kBytesPerKilobyte ? QStringLiteral("KB") : unit == megabyte ? QStringLiteral("MB") : QStringLiteral("GB");
    auto number = QString::number(static_cast<double>(bytes) / static_cast<double>(unit), 'f', 1);
    if (number.endsWith(QStringLiteral(".0"))) number.chop(2);
    return QStringLiteral("%1 %2").arg(number, name);
}
}
