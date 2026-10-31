// [0.2.0 6.8 7] specs/features/0.2.0-fcflash.md
#include <kgc/fcFlash/Errors.h>
#include <QCoreApplication>
namespace kgc::fcFlash {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    const char* key = nullptr;
    switch (static_cast<FcFlashError>(error.code)) {
    case FcFlashError::Armed: key = QT_TRANSLATE_NOOP("FcFlash", "시동 중에는 구울 수 없어요."); break;
    case FcFlashError::OtherLink: key = QT_TRANSLATE_NOOP("FcFlash", "다른 연결을 먼저 끊어요."); break;
    case FcFlashError::BootloaderNotFound: key = QT_TRANSLATE_NOOP("FcFlash", "부트로더를 찾지 못했어요. USB를 다시 꽂고 다시 해요."); break;
    case FcFlashError::BadBootloaderRevision: key = QT_TRANSLATE_NOOP("FcFlash", "지원하지 않는 부트로더예요."); break;
    case FcFlashError::TooLarge: key = QT_TRANSLATE_NOOP("FcFlash", "펌웨어가 FC 플래시보다 커요."); break;
    case FcFlashError::BadImage: key = QT_TRANSLATE_NOOP("FcFlash", "펌웨어 파일을 읽을 수 없어요."); break;
    case FcFlashError::DownloadFailed: key = QT_TRANSLATE_NOOP("FcFlash", "펌웨어를 내려받지 못했어요."); break;
    case FcFlashError::PortLost: key = QT_TRANSLATE_NOOP("FcFlash", "포트를 잃었어요. USB를 다시 꽂고 처음부터 해요."); break;
    case FcFlashError::Aborted: key = QT_TRANSLATE_NOOP("FcFlash", "취소했어요."); break;
    case FcFlashError::WrongBoard: return {};
    case FcFlashError::EraseFailed:
    case FcFlashError::ProgramFailed:
    case FcFlashError::VerifyFailed: {
        const char* stage = error == make(FcFlashError::EraseFailed) ? QT_TRANSLATE_NOOP("FcFlash", "지우기")
            : error == make(FcFlashError::ProgramFailed) ? QT_TRANSLATE_NOOP("FcFlash", "쓰기") : QT_TRANSLATE_NOOP("FcFlash", "검증");
        const auto format = QT_TRANSLATE_NOOP("FcFlash", "%1에 실패했어요. FC는 부트로더에서 기다리고 있어요.");
        return QCoreApplication::translate("FcFlash", format)
            .arg(QCoreApplication::translate("FcFlash", stage));
    }
    }
    return key ? QCoreApplication::translate("FcFlash", key) : QString{};
}
}
