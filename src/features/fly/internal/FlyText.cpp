// [0.6.0 5.11.3 6 T9] specs/features/0.6.0-fly.md
#include "FlyText.h"
#include <QCoreApplication>
#include <cmath>
namespace kgc::fly {
QString distanceText(double meters)
{
    if (std::isnan(meters)) return QStringLiteral("—");
    if (qRound(meters) < 1000) return QStringLiteral("%1 m").arg(qRound(meters));
    return QStringLiteral("%1 km").arg(lengthNumber(meters / 1000));
}
QString lengthNumber(double meters)
{
    auto text = QString::number(meters, 'f', 1);
    if (text.endsWith(QStringLiteral(".0"))) text.chop(2);
    return text;
}
QString positionText(double latDeg, double lonDeg)
{
    return QString::number(latDeg, 'f', 6) + QStringLiteral(", ") + QString::number(lonDeg, 'f', 6);
}
QString actionName(FlyActionKind kind)
{
    switch (kind) {
    case FlyActionKind::Arm: return QStringLiteral("Arm");
    case FlyActionKind::Disarm: return QStringLiteral("Disarm");
    case FlyActionKind::EmergencyStop: return QStringLiteral("EmergencyStop");
    case FlyActionKind::Takeoff: return QStringLiteral("Takeoff");
    case FlyActionKind::Land: return QStringLiteral("Land");
    case FlyActionKind::Rtl: return QStringLiteral("Rtl");
    case FlyActionKind::Pause: return QStringLiteral("Pause");
    case FlyActionKind::SetMode: return QStringLiteral("SetMode");
    case FlyActionKind::GoTo: return QStringLiteral("GoTo");
    }
    return {};
}
QString flyErrorName(FlyError error)
{
    switch (error) {
    case FlyError::NoLink: return QStringLiteral("NoLink");
    case FlyError::NoAck: return QStringLiteral("NoAck");
    case FlyError::Denied: return QStringLiteral("Denied");
    case FlyError::Unsupported: return QStringLiteral("Unsupported");
    case FlyError::TemporarilyRejected: return QStringLiteral("TemporarilyRejected");
    case FlyError::Failed: return QStringLiteral("Failed");
    case FlyError::ModeNotChanged: return QStringLiteral("ModeNotChanged");
    case FlyError::ArmNotChanged: return QStringLiteral("ArmNotChanged");
    case FlyError::TargetRejected: return QStringLiteral("TargetRejected");
    case FlyError::Busy: return QStringLiteral("Busy");
    case FlyError::NotReady: return QStringLiteral("NotReady");
    case FlyError::HomeUnknown: return QStringLiteral("HomeUnknown");
    case FlyError::PositionUnknown: return QStringLiteral("PositionUnknown");
    case FlyError::TooFar: return QStringLiteral("TooFar");
    case FlyError::AltitudeOutOfRange: return QStringLiteral("AltitudeOutOfRange");
    case FlyError::Preempted: return QStringLiteral("Preempted");
    case FlyError::PolicyDenied: return QStringLiteral("PolicyDenied");
    case FlyError::Gone: return QStringLiteral("Gone");
    case FlyError::TargetWithoutMode: return QStringLiteral("TargetWithoutMode");
    }
    return {};
}
QString actionFailureText(FlyActionKind kind)
{
    const char* key = nullptr;
    switch (kind) {
    case FlyActionKind::Arm: key = QT_TRANSLATE_NOOP("Fly", "시동 실패"); break;
    case FlyActionKind::Disarm: key = QT_TRANSLATE_NOOP("Fly", "시동 끄기 실패"); break;
    case FlyActionKind::EmergencyStop: key = QT_TRANSLATE_NOOP("Fly", "비상 정지 실패"); break;
    case FlyActionKind::Takeoff: key = QT_TRANSLATE_NOOP("Fly", "이륙 실패"); break;
    case FlyActionKind::Land: key = QT_TRANSLATE_NOOP("Fly", "착륙 시작 실패"); break;
    case FlyActionKind::Rtl: key = QT_TRANSLATE_NOOP("Fly", "RTL 시작 실패"); break;
    case FlyActionKind::Pause: key = QT_TRANSLATE_NOOP("Fly", "일시정지 실패"); break;
    case FlyActionKind::SetMode: key = QT_TRANSLATE_NOOP("Fly", "모드 변경 실패"); break;
    case FlyActionKind::GoTo: key = QT_TRANSLATE_NOOP("Fly", "목표 이동 실패"); break;
    }
    return key ? QCoreApplication::translate("Fly", key) : QString{};
}
}
