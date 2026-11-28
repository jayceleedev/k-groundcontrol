// [0.5.0 5.6.2 6] specs/features/0.5.0-map.md
#include <kgc/map/Errors.h>
#include <QCoreApplication>
namespace kgc::map {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    const char* key = nullptr;
    switch (static_cast<MapError>(error.code)) {
    case MapError::TooManyTiles:
        key = QT_TRANSLATE_NOOP("Map", "한 번에 최대 타일 100,000개. 영역이나 확대 범위 축소 필요");
        break;
    case MapError::BadZoomRange: key = QT_TRANSLATE_NOOP("Map", "잘못된 확대 범위"); break;
    case MapError::DownloadBusy: key = QT_TRANSLATE_NOOP("Map", "다른 영역 받는 중"); break;
    case MapError::TileFailed: key = QT_TRANSLATE_NOOP("Map", "타일 받기 실패"); break;
    case MapError::StoreFailed: key = QT_TRANSLATE_NOOP("Map", "지도 저장소 사용 실패"); break;
    }
    return key ? QCoreApplication::translate("Map", key) : QString{};
}
}
