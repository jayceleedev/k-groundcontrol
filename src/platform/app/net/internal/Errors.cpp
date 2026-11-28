// [0.5.0 S2 6] specs/features/0.5.0-map.md
// [0.2.0 7] specs/features/0.2.0-fcflash.md
#include <kgc/app/net/Errors.h>
#include <QCoreApplication>
namespace kgc::app::net {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    switch (static_cast<HttpError>(error.code)) {
    case HttpError::NotHttps: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "HTTPS 주소만 지원"));
    case HttpError::TimedOut: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "요청 시간 초과"));
    case HttpError::RedirectRefused: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "받기 실패"));
    case HttpError::NetworkFailed: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "받기 실패"));
    case HttpError::WriteFailed: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "받기 실패"));
    case HttpError::Aborted: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "받기 실패"));
    case HttpError::BadGzip: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "받기 실패"));
    case HttpError::BadCertificate:
        return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "인증서 파일 읽기 실패"));
    case HttpError::HttpStatus:
        return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "서버가 요청 거부"));
    }
    return {};
}
}
