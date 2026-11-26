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
        key = QT_TRANSLATE_NOOP("Map", "한 번에 100,000개까지 받을 수 있어요. 영역이나 확대 범위를 줄여 주세요.");
        break;
    case MapError::BadZoomRange: key = QT_TRANSLATE_NOOP("Map", "확대 범위가 맞지 않아요"); break;
    case MapError::DownloadBusy: key = QT_TRANSLATE_NOOP("Map", "다른 영역을 받는 중이에요"); break;
    case MapError::TileFailed: key = QT_TRANSLATE_NOOP("Map", "타일을 받지 못했어요"); break;
    case MapError::StoreFailed: key = QT_TRANSLATE_NOOP("Map", "지도 저장소를 쓰지 못했어요"); break;
    }
    return key ? QCoreApplication::translate("Map", key) : QString{};
}
}
