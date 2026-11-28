// [0.5.0 S1 6] specs/features/0.5.0-map.md
#include <kgc/app/store/Errors.h>
#include <QCoreApplication>
namespace kgc::app::store {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    switch (static_cast<StoreError>(error.code)) {
    case StoreError::OpenFailed: return QCoreApplication::translate("Store", QT_TRANSLATE_NOOP("Store", "저장소 열기 실패"));
    case StoreError::QueryFailed:
        return QCoreApplication::translate("Store", QT_TRANSLATE_NOOP("Store", "저장소 읽기 또는 쓰기 실패"));
    }
    return {};
}
}
