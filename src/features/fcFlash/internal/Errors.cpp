// [0.2.0 6.8 7] specs/features/0.2.0-fcflash.md
#include <kgc/fcFlash/Errors.h>
#include <QCoreApplication>
namespace kgc::fcFlash {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    const char* key = nullptr;
    switch (static_cast<FcFlashError>(error.code)) {
    case FcFlashError::Armed: key = QT_TRANSLATE_NOOP("FcFlash", "기체 시동 중 굽기 불가"); break;
    case FcFlashError::OtherLink: key = QT_TRANSLATE_NOOP("FcFlash", "다른 링크 먼저 연결 해제"); break;
    case FcFlashError::BootloaderNotFound: key = QT_TRANSLATE_NOOP("FcFlash", "부트로더 찾기 실패. USB 재연결 후 재시도"); break;
    case FcFlashError::BadBootloaderRevision: key = QT_TRANSLATE_NOOP("FcFlash", "지원하지 않는 부트로더"); break;
    case FcFlashError::TooLarge: key = QT_TRANSLATE_NOOP("FcFlash", "펌웨어가 FC 플래시 용량 초과"); break;
    case FcFlashError::BadImage: key = QT_TRANSLATE_NOOP("FcFlash", "펌웨어 파일 읽기 실패"); break;
    case FcFlashError::DownloadFailed: key = QT_TRANSLATE_NOOP("FcFlash", "펌웨어 받기 실패"); break;
    case FcFlashError::PortLost: key = QT_TRANSLATE_NOOP("FcFlash", "포트 연결 끊김. USB 재연결 후 처음부터 시작"); break;
    case FcFlashError::Aborted: key = QT_TRANSLATE_NOOP("FcFlash", "취소 완료"); break;
    case FcFlashError::WrongBoard: return {};
    case FcFlashError::EraseFailed:
    case FcFlashError::ProgramFailed:
    case FcFlashError::VerifyFailed: {
        const char* stage = error == make(FcFlashError::EraseFailed) ? QT_TRANSLATE_NOOP("FcFlash", "지우기")
            : error == make(FcFlashError::ProgramFailed) ? QT_TRANSLATE_NOOP("FcFlash", "쓰기") : QT_TRANSLATE_NOOP("FcFlash", "검증");
        const auto format = QT_TRANSLATE_NOOP("FcFlash", "%1 실패. FC는 부트로더에서 대기 중");
        return QCoreApplication::translate("FcFlash", format)
            .arg(QCoreApplication::translate("FcFlash", stage));
    }
    }
    return key ? QCoreApplication::translate("FcFlash", key) : QString{};
}
}
