// [0.5.0 5.6.3 T1] specs/features/0.5.0-map.md
#include "TileUrl.h"
namespace kgc::map {
namespace {
// Google 타일 주소의 lyrs 값이에요(s 위성, y 하이브리드, m 일반).
QString lyrs(settings::MapType type)
{
    switch (type) {
    case settings::MapType::Satellite: return QStringLiteral("s");
    case settings::MapType::Hybrid: return QStringLiteral("y");
    case settings::MapType::Road: return QStringLiteral("m");
    }
    return QStringLiteral("s");
}
}
QString tileUrl(settings::MapType type, const QString& language, int z, int x, int y, const QString& host)
{
    // 서버 넷(mt0~mt3)에 고르게 나눠 보내요.
    const auto base = host.isEmpty() ? QStringLiteral("https://mt%1.google.com").arg((x + y) % 4) : host;
    return base + QStringLiteral("/vt/lyrs=%1&hl=%2&x=%3&y=%4&z=%5")
        .arg(lyrs(type), language, QString::number(x), QString::number(y), QString::number(z));
}
QString tileKey(settings::MapType type, const QString& language, int z, int x, int y)
{
    const auto tail = QStringLiteral("%1/%2/%3").arg(z).arg(x).arg(y);
    if (type == settings::MapType::Satellite) return lyrs(type) + u'/' + tail;
    return lyrs(type) + u'/' + language + u'/' + tail;
}
}
