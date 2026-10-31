// [0.2.0 7] specs/features/0.2.0-fcflash.md
#include <kgc/app/net/Errors.h>
#include <QCoreApplication>
namespace kgc::app::net {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    switch (static_cast<HttpError>(error.code)) {
    case HttpError::NotHttps: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "HTTPS 주소만 쓸 수 있어요."));
    case HttpError::TimedOut: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "요청 시간이 지났어요."));
    case HttpError::RedirectRefused: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "내려받지 못했어요."));
    case HttpError::NetworkFailed: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "내려받지 못했어요."));
    case HttpError::WriteFailed: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "내려받지 못했어요."));
    case HttpError::Aborted: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "내려받지 못했어요."));
    case HttpError::BadGzip: return QCoreApplication::translate("Http", QT_TRANSLATE_NOOP("Http", "내려받지 못했어요."));
    }
    return {};
}
}
