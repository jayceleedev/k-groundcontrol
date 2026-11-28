// [0.6.0 5.4] specs/features/0.6.0-fly.md
#pragma once
#include <QObject>
#include <limits>
namespace kgc::vehicle::states {
/** 기체가 지금 향하는 항법 목표와 선회 상태를 전달해요.
 * 입력: POSITION_TARGET_GLOBAL_INT(목표 셋)와 ORBIT_EXECUTION_STATUS(선회 셋)예요.
 * 출력: latDeg·lonDeg는 도, altM은 기체가 보낸 프레임 그대로의 고도(m, 0.6.0은 쓰지 않음)예요.
 *       orbitRadiusM은 부호 있는 선회 반지름(m, 양수 시계), orbitLatDeg·orbitLonDeg는 선회 중심(도)이에요. 모르는 값은 NaN이고 3초 안 오면 다시 NaN이에요.
 *       targetCount는 POSITION_TARGET_GLOBAL_INT를 받을 때마다 1 늘어요(처음 0). 선회·만료로는 바뀌지 않아 새 목표 메시지와 다른 까닭의 알림을 가를 수 있어요.
 * 실패: 없어요.
 */
struct NavTargetBundle {
    Q_GADGET
    Q_PROPERTY(double latDeg MEMBER latDeg)
    Q_PROPERTY(double lonDeg MEMBER lonDeg)
    Q_PROPERTY(double altM MEMBER altM)
    Q_PROPERTY(double orbitRadiusM MEMBER orbitRadiusM)
    Q_PROPERTY(double orbitLatDeg MEMBER orbitLatDeg)
    Q_PROPERTY(double orbitLonDeg MEMBER orbitLonDeg)
    Q_PROPERTY(int targetCount MEMBER targetCount)
public:
    double latDeg = std::numeric_limits<double>::quiet_NaN();
    double lonDeg = std::numeric_limits<double>::quiet_NaN();
    double altM = std::numeric_limits<double>::quiet_NaN();
    double orbitRadiusM = std::numeric_limits<double>::quiet_NaN();
    double orbitLatDeg = std::numeric_limits<double>::quiet_NaN();
    double orbitLonDeg = std::numeric_limits<double>::quiet_NaN();
    int targetCount = 0;
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::NavTargetBundle)
