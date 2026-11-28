// [0.6.0 5.11.2 6] specs/features/0.6.0-fly.md
#include <kgc/fly/Errors.h>
#include <QCoreApplication>
namespace kgc::fly {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    const char* key = nullptr;
    switch (static_cast<FlyError>(error.code)) {
    case FlyError::NoLink: key = QT_TRANSLATE_NOOP("Fly", "기체와 연결이 없어요"); break;
    case FlyError::NoAck: key = QT_TRANSLATE_NOOP("Fly", "기체가 응답하지 않았어요"); break;
    case FlyError::Denied: key = QT_TRANSLATE_NOOP("Fly", "기체가 거부했어요"); break;
    case FlyError::Unsupported: key = QT_TRANSLATE_NOOP("Fly", "기체가 이 명령을 지원하지 않아요"); break;
    case FlyError::TemporarilyRejected: key = QT_TRANSLATE_NOOP("Fly", "기체가 지금은 받을 수 없다고 했어요"); break;
    case FlyError::Failed: key = QT_TRANSLATE_NOOP("Fly", "기체가 실행하지 못했어요"); break;
    case FlyError::ModeNotChanged: key = QT_TRANSLATE_NOOP("Fly", "기체 모드가 바뀌지 않았어요"); break;
    case FlyError::ArmNotChanged: key = QT_TRANSLATE_NOOP("Fly", "기체 시동 상태가 바뀌지 않았어요"); break;
    case FlyError::TargetRejected: key = QT_TRANSLATE_NOOP("Fly", "기체가 목표를 받지 않았어요"); break;
    case FlyError::Busy: key = QT_TRANSLATE_NOOP("Fly", "다른 비행 명령을 보내는 중이에요"); break;
    case FlyError::NotReady: key = QT_TRANSLATE_NOOP("Fly", "지금은 할 수 없어요"); break;
    case FlyError::HomeUnknown: key = QT_TRANSLATE_NOOP("Fly", "홈 고도를 아직 몰라요"); break;
    case FlyError::PositionUnknown: key = QT_TRANSLATE_NOOP("Fly", "기체 위치를 아직 몰라요"); break;
    case FlyError::TooFar: key = QT_TRANSLATE_NOOP("Fly", "기체에서 너무 멀어요"); break;
    case FlyError::AltitudeOutOfRange: key = QT_TRANSLATE_NOOP("Fly", "고도가 허용 범위 밖이에요"); break;
    case FlyError::Preempted: key = QT_TRANSLATE_NOOP("Fly", "시동 끄기나 비상 정지 때문에 멈췄어요"); break;
    case FlyError::PolicyDenied: key = QT_TRANSLATE_NOOP("Fly", "지금은 이 명령을 보낼 수 없어요"); break;
    case FlyError::Gone: key = QT_TRANSLATE_NOOP("Fly", "기체가 사라졌어요"); break;
    case FlyError::TargetWithoutMode:
        key = QT_TRANSLATE_NOOP("Fly", "기체가 목표를 받았지만 모드는 그대로예요. 그곳으로 갈 수 있어요");
        break;
    }
    return key ? QCoreApplication::translate("Fly", key) : QString{};
}
}
