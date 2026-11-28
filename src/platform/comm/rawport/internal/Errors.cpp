// [0.2.0 7] specs/features/0.2.0-fcflash.md
#include <kgc/comm/rawport/Errors.h>
#include <QCoreApplication>
namespace kgc::comm::rawport {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    switch (static_cast<PortError>(error.code)) {
    case PortError::Busy: return QCoreApplication::translate("Port", QT_TRANSLATE_NOOP("Port", "다른 작업이 포트 사용 중"));
    case PortError::OpenFailed: return QCoreApplication::translate("Port", QT_TRANSLATE_NOOP("Port", "포트 연결 끊김"));
    case PortError::Lost: return QCoreApplication::translate("Port", QT_TRANSLATE_NOOP("Port", "포트 연결 끊김"));
    case PortError::Timeout: return QCoreApplication::translate("Port", QT_TRANSLATE_NOOP("Port", "포트 응답 없음"));
    }
    return {};
}
}
