// [0.4.0 7] specs/features/0.4.0-sitl.md
#include <kgc/sitl/Errors.h>
#include <QCoreApplication>
namespace kgc::sitl {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    const char* key = nullptr;
    switch (static_cast<SitlError>(error.code)) {
    case SitlError::LinkBusy: key = QT_TRANSLATE_NOOP("Sitl", "연결을 끊은 뒤 시작할 수 있어요"); break;
    case SitlError::LinkFailed: key = QT_TRANSLATE_NOOP("Sitl", "UDP 14550을 열지 못했어요. 다른 프로그램이 쓰고 있는지 확인해 주세요"); break;
    case SitlError::DownloadFailed: key = QT_TRANSLATE_NOOP("Sitl", "SITL 파일을 받지 못했어요. 인터넷 연결을 확인해 주세요"); break;
    case SitlError::StartFailed: key = QT_TRANSLATE_NOOP("Sitl", "시뮬레이터를 실행하지 못했어요"); break;
    case SitlError::Crashed: key = QT_TRANSLATE_NOOP("Sitl", "시뮬레이터가 멈췄어요"); break;
    case SitlError::BadHome: key = QT_TRANSLATE_NOOP("Sitl", "홈 위치가 범위를 벗어났어요"); break;
    case SitlError::Busy: key = QT_TRANSLATE_NOOP("Sitl", "다른 작업이 끝난 뒤 다시 해 주세요"); break;
    }
    return key ? QCoreApplication::translate("Sitl", key) : QString{};
}
}
