// [0.5.1 5.3 T2] specs/features/0.5.1-map-sharp.md
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
QString tileUrl(settings::MapType type, const QString& language, int ratio, int z, int x, int y, const QString& host)
{
    Q_ASSERT(ratio == 1 || ratio == 2);
    // 서버 넷(mt0~mt3)에 고르게 나눠 보내요.
    const auto base = host.isEmpty() ? QStringLiteral("https://mt%1.google.com").arg((x + y) % 4) : host;
    const auto url = base + QStringLiteral("/vt/lyrs=%1&hl=%2&x=%3&y=%4&z=%5")
        .arg(lyrs(type), language, QString::number(x), QString::number(y), QString::number(z));
    // 2배 타일만 뒤에 붙여요. 1배 주소는 0.5.0과 같아요.
    return ratio == 2 ? url + QStringLiteral("&scale=2") : url;
}
QString tileKey(settings::MapType type, const QString& language, int ratio, int z, int x, int y)
{
    Q_ASSERT(ratio == 1 || ratio == 2);
    const auto tail = QStringLiteral("%1/%2/%3").arg(z).arg(x).arg(y);
    // 2배 타일만 뒤에 붙여요. 1배 열쇠는 0.5.0과 같아 저장된 행과 받은 영역이 그대로 쓰여요.
    const auto suffix = ratio == 2 ? QStringLiteral("@2") : QString();
    if (type == settings::MapType::Satellite) return lyrs(type) + u'/' + tail + suffix;
    return lyrs(type) + u'/' + language + u'/' + tail + suffix;
}
}
