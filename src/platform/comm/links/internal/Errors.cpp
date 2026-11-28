// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/links/Errors.h>
namespace kgc::comm::links {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    switch (static_cast<LinkError>(error.code)) {
    case LinkError::AlreadyConnected: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "이미 연결 중"));
    case LinkError::PortOpenFailed: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "포트 열기 실패"));
    case LinkError::PermissionDenied: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "포트 접근 권한 없음 또는 다른 프로그램이 사용 중"));
    case LinkError::PortBusy: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "다른 프로그램이 포트 사용 중"));
    case LinkError::UdpBindFailed: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "UDP 수신 포트 열기 실패"));
    case LinkError::PortLost: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "포트 연결 끊김"));
    }
    return {};
}
}
