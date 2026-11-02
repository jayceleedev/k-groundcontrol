// [0.3.0 7] specs/features/0.3.0-params.md
#include <kgc/params/Errors.h>
#include <QCoreApplication>
namespace kgc::params {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    const char* key = nullptr;
    switch (static_cast<ParamError>(error.code)) {
    case ParamError::Timeout: key = QT_TRANSLATE_NOOP("Params", "FC가 응답하지 않아요"); break;
    case ParamError::Incomplete: key = QT_TRANSLATE_NOOP("Params", "FC가 파라미터를 다 보내지 않았어요"); break;
    case ParamError::BackupFailed: key = QT_TRANSLATE_NOOP("Params", "백업을 저장하지 못해 쓰지 않았어요"); break;
    case ParamError::Armed: key = QT_TRANSLATE_NOOP("Params", "시동 중에는 쓸 수 없어요"); break;
    case ParamError::NotFound: key = QT_TRANSLATE_NOOP("Params", "이 파라미터는 FC에 없어요"); break;
    case ParamError::NotANumber: key = QT_TRANSLATE_NOOP("Params", "숫자를 넣어 주세요"); break;
    case ParamError::NotInteger: key = QT_TRANSLATE_NOOP("Params", "정수를 넣어 주세요"); break;
    case ParamError::TypeLimit: key = QT_TRANSLATE_NOOP("Params", "이 파라미터가 담을 수 없는 값이에요"); break;
    case ParamError::ReadOnly: key = QT_TRANSLATE_NOOP("Params", "읽기 전용이에요"); break;
    case ParamError::FileOpen: key = QT_TRANSLATE_NOOP("Params", "파일을 열지 못했어요"); break;
    case ParamError::FileEmpty: key = QT_TRANSLATE_NOOP("Params", "파일에 읽을 값이 없어요"); break;
    case ParamError::FileWrite: key = QT_TRANSLATE_NOOP("Params", "파일을 저장하지 못했어요"); break;
    case ParamError::Busy: key = QT_TRANSLATE_NOOP("Params", "다른 작업이 끝난 뒤 다시 해 주세요"); break;
    case ParamError::RebootRefused: key = QT_TRANSLATE_NOOP("Params", "FC가 재부팅을 거부했어요"); break;
    }
    return key ? QCoreApplication::translate("Params", key) : QString{};
}
}
