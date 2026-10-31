// [0.2.0 7] specs/features/0.2.0-fcflash.md
#include <kgc/comm/rawport/Errors.h>
#include <QCoreApplication>
namespace kgc::comm::rawport {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    switch (static_cast<PortError>(error.code)) {
    case PortError::Busy: return QCoreApplication::translate("Port", QT_TRANSLATE_NOOP("Port", "다른 작업이 포트를 쓰고 있어요."));
    case PortError::OpenFailed: return QCoreApplication::translate("Port", QT_TRANSLATE_NOOP("Port", "포트를 잃었어요."));
    case PortError::Lost: return QCoreApplication::translate("Port", QT_TRANSLATE_NOOP("Port", "포트를 잃었어요."));
    case PortError::Timeout: return QCoreApplication::translate("Port", QT_TRANSLATE_NOOP("Port", "포트가 응답하지 않아요."));
    }
    return {};
}
}
