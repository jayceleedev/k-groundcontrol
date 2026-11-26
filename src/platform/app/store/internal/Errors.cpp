// [0.5.0 S1 6] specs/features/0.5.0-map.md
#include <kgc/app/store/Errors.h>
#include <QCoreApplication>
namespace kgc::app::store {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    switch (static_cast<StoreError>(error.code)) {
    case StoreError::OpenFailed: return QCoreApplication::translate("Store", QT_TRANSLATE_NOOP("Store", "저장소를 열지 못했어요"));
    case StoreError::QueryFailed:
        return QCoreApplication::translate("Store", QT_TRANSLATE_NOOP("Store", "저장소를 읽거나 쓰지 못했어요"));
    }
    return {};
}
}
