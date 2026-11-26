// [0.5.0 5.6.3 T1] specs/features/0.5.0-map.md
#include "TileImage.h"
namespace kgc::map {
QString tileImageFormat(const QByteArray& bytes)
{
    // 머리 바이트는 문자열로 비교해요(긴 16진 정수는 고정값 검사 대상이에요).
    if (bytes.startsWith("\xFF\xD8\xFF")) return QStringLiteral("jpeg");
    if (bytes.startsWith("\x89PNG\r\n\x1A\n")) return QStringLiteral("png");
    return {};
}
}
