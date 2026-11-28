// [0.6.0 5.11.2 6] specs/features/0.6.0-fly.md
#include <kgc/fly/Errors.h>
#include <QCoreApplication>
namespace kgc::fly {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    const char* key = nullptr;
    switch (static_cast<FlyError>(error.code)) {
    case FlyError::NoLink: key = QT_TRANSLATE_NOOP("Fly", "기체 연결 없음"); break;
    case FlyError::NoAck: key = QT_TRANSLATE_NOOP("Fly", "기체 응답 없음"); break;
    case FlyError::Denied: key = QT_TRANSLATE_NOOP("Fly", "기체가 거부"); break;
    case FlyError::Unsupported: key = QT_TRANSLATE_NOOP("Fly", "기체가 지원하지 않는 명령"); break;
    case FlyError::TemporarilyRejected: key = QT_TRANSLATE_NOOP("Fly", "기체가 현재 수락 불가"); break;
    case FlyError::Failed: key = QT_TRANSLATE_NOOP("Fly", "기체에서 실행 안 됨"); break;
    case FlyError::ModeNotChanged: key = QT_TRANSLATE_NOOP("Fly", "기체 모드 변경 안 됨"); break;
    case FlyError::ArmNotChanged: key = QT_TRANSLATE_NOOP("Fly", "기체 시동 상태 변경 안 됨"); break;
    case FlyError::TargetRejected: key = QT_TRANSLATE_NOOP("Fly", "기체가 목표 미수락"); break;
    case FlyError::Busy: key = QT_TRANSLATE_NOOP("Fly", "다른 비행 명령 진행 중"); break;
    case FlyError::NotReady: key = QT_TRANSLATE_NOOP("Fly", "현재 불가"); break;
    case FlyError::HomeUnknown: key = QT_TRANSLATE_NOOP("Fly", "홈 고도 아직 모름"); break;
    case FlyError::PositionUnknown: key = QT_TRANSLATE_NOOP("Fly", "기체 위치 아직 모름"); break;
    case FlyError::TooFar: key = QT_TRANSLATE_NOOP("Fly", "기체에서 너무 먼 위치"); break;
    case FlyError::AltitudeOutOfRange: key = QT_TRANSLATE_NOOP("Fly", "고도 허용 범위 밖"); break;
    case FlyError::Preempted: key = QT_TRANSLATE_NOOP("Fly", "시동 끄기 또는 비상 정지로 중단"); break;
    case FlyError::PolicyDenied: key = QT_TRANSLATE_NOOP("Fly", "현재 이 명령 전송 불가"); break;
    case FlyError::Gone: key = QT_TRANSLATE_NOOP("Fly", "기체 사라짐"); break;
    case FlyError::TargetWithoutMode:
        key = QT_TRANSLATE_NOOP("Fly", "기체가 목표 수락, 모드는 유지. 목표로 비행할 수도 있음");
        break;
    }
    return key ? QCoreApplication::translate("Fly", key) : QString{};
}
}
