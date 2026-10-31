// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/links/Errors.h>
namespace kgc::comm::links {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    switch (static_cast<LinkError>(error.code)) {
    case LinkError::AlreadyConnected: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "이미 연결 중이에요."));
    case LinkError::PortOpenFailed: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "포트를 열 수 없어요."));
    case LinkError::PermissionDenied: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "포트를 열 권한이 없거나 다른 프로그램이 쓰고 있어요."));
    case LinkError::PortBusy: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "다른 프로그램이 포트를 쓰고 있어요."));
    case LinkError::UdpBindFailed: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "UDP 수신 포트를 열 수 없어요."));
    case LinkError::PortLost: return QCoreApplication::translate("Link", QT_TRANSLATE_NOOP("Link", "포트 연결이 끊어졌어요."));
    }
    return {};
}
}
