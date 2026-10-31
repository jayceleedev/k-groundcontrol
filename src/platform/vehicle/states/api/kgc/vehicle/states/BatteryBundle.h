// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QObject>
#include <QString>
#include <limits>
namespace kgc::vehicle::states {
/** 기체의 배터리 전압과 전류, 잔량을 전달해요.
 * 입력: SYS_STATUS에서 변환한 voltageV·currentA·remainingPct예요.
 * 출력: 전압은 볼트, 전류는 암페어, 잔량은 백분율이며 미확인은 NaN·NaN·-1이에요.
 * 실패: 없어요.
 */
struct BatteryBundle {
    Q_GADGET
    Q_PROPERTY(double voltageV MEMBER voltageV)
    Q_PROPERTY(double currentA MEMBER currentA)
    Q_PROPERTY(int remainingPct MEMBER remainingPct)
public:
    double voltageV = std::numeric_limits<double>::quiet_NaN();
    double currentA = std::numeric_limits<double>::quiet_NaN();
    int remainingPct = -1;
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::BatteryBundle)
