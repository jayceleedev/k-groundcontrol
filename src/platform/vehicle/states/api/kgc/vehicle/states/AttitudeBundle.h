// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QObject>
#include <QString>
#include <limits>
namespace kgc::vehicle::states {
/** 기체 자세를 도 단위로 전달해요.
 * 입력: ATTITUDE에서 변환한 rollDeg·pitchDeg·yawDeg예요.
 * 출력: 방위는 0~360 범위이며, 수신 전 세 값은 NaN이에요.
 * 실패: 없어요.
 */
struct AttitudeBundle {
    Q_GADGET
    Q_PROPERTY(double rollDeg MEMBER rollDeg)
    Q_PROPERTY(double pitchDeg MEMBER pitchDeg)
    Q_PROPERTY(double yawDeg MEMBER yawDeg)
public:
    double rollDeg = std::numeric_limits<double>::quiet_NaN();
    double pitchDeg = std::numeric_limits<double>::quiet_NaN();
    double yawDeg = std::numeric_limits<double>::quiet_NaN();
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::AttitudeBundle)
