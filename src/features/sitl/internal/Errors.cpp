// [0.4.0 7] specs/features/0.4.0-sitl.md
#include <kgc/sitl/Errors.h>
#include <QCoreApplication>
namespace kgc::sitl {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    const char* key = nullptr;
    switch (static_cast<SitlError>(error.code)) {
    case SitlError::LinkBusy: key = QT_TRANSLATE_NOOP("Sitl", "연결 해제 후 시작 가능"); break;
    case SitlError::LinkFailed: key = QT_TRANSLATE_NOOP("Sitl", "UDP 14550 열기 실패. 다른 프로그램이 사용 중인지 확인"); break;
    case SitlError::DownloadFailed: key = QT_TRANSLATE_NOOP("Sitl", "SITL 파일 받기 실패. 인터넷 연결 확인"); break;
    case SitlError::StartFailed: key = QT_TRANSLATE_NOOP("Sitl", "시뮬레이터 실행 실패"); break;
    case SitlError::Crashed: key = QT_TRANSLATE_NOOP("Sitl", "시뮬레이터가 멈춤"); break;
    case SitlError::BadHome: key = QT_TRANSLATE_NOOP("Sitl", "홈 위치 범위 밖"); break;
    case SitlError::Busy: key = QT_TRANSLATE_NOOP("Sitl", "다른 작업 완료 후 재시도"); break;
    }
    return key ? QCoreApplication::translate("Sitl", key) : QString{};
}
}
