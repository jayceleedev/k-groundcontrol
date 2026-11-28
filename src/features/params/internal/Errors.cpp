// [0.3.0 7] specs/features/0.3.0-params.md
#include <kgc/params/Errors.h>
#include <QCoreApplication>
namespace kgc::params {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    const char* key = nullptr;
    switch (static_cast<ParamError>(error.code)) {
    case ParamError::Timeout: key = QT_TRANSLATE_NOOP("Params", "FC 응답 없음"); break;
    case ParamError::Incomplete: key = QT_TRANSLATE_NOOP("Params", "FC가 파라미터 일부 미전송"); break;
    case ParamError::BackupFailed: key = QT_TRANSLATE_NOOP("Params", "백업 저장 실패로 쓰기 미실행"); break;
    case ParamError::Armed: key = QT_TRANSLATE_NOOP("Params", "시동 중 쓰기 불가"); break;
    case ParamError::NotFound: key = QT_TRANSLATE_NOOP("Params", "FC에 해당 파라미터 없음"); break;
    case ParamError::NotANumber: key = QT_TRANSLATE_NOOP("Params", "숫자 입력"); break;
    case ParamError::NotInteger: key = QT_TRANSLATE_NOOP("Params", "정수 입력"); break;
    case ParamError::TypeLimit: key = QT_TRANSLATE_NOOP("Params", "이 파라미터에 저장할 수 없는 값"); break;
    case ParamError::ReadOnly: key = QT_TRANSLATE_NOOP("Params", "읽기 전용 파라미터"); break;
    case ParamError::FileOpen: key = QT_TRANSLATE_NOOP("Params", "파일 열기 실패"); break;
    case ParamError::FileEmpty: key = QT_TRANSLATE_NOOP("Params", "파일에 읽을 값 없음"); break;
    case ParamError::FileWrite: key = QT_TRANSLATE_NOOP("Params", "파일 저장 실패"); break;
    case ParamError::Busy: key = QT_TRANSLATE_NOOP("Params", "현재 작업 완료 후 재시도"); break;
    case ParamError::RebootRefused: key = QT_TRANSLATE_NOOP("Params", "FC가 재부팅 거부"); break;
    }
    return key ? QCoreApplication::translate("Params", key) : QString{};
}
}
