// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QObject>
#include <QString>
#include <limits>
namespace kgc::vehicle::states {
/** 기체 위치와 속도를 화면에서 쓰는 단위로 전달해요.
 * 입력: GLOBAL_POSITION_INT에서 변환한 위치·고도·수평 속도·상승률·헤딩이에요.
 * 출력: 위경도와 헤딩은 도, 고도는 미터, 속도는 초당 미터이며 모르는 값은 NaN이에요.
 * 실패: 없어요.
 */
struct PositionBundle {
    Q_GADGET
    Q_PROPERTY(double latDeg MEMBER latDeg)
    Q_PROPERTY(double lonDeg MEMBER lonDeg)
    Q_PROPERTY(double altAmslM MEMBER altAmslM)
    Q_PROPERTY(double altRelM MEMBER altRelM)
    Q_PROPERTY(double groundSpeedMs MEMBER groundSpeedMs)
    Q_PROPERTY(double climbMs MEMBER climbMs)
    Q_PROPERTY(double headingDeg MEMBER headingDeg)
public:
    double latDeg = std::numeric_limits<double>::quiet_NaN();
    double lonDeg = std::numeric_limits<double>::quiet_NaN();
    double altAmslM = std::numeric_limits<double>::quiet_NaN();
    double altRelM = std::numeric_limits<double>::quiet_NaN();
    double groundSpeedMs = std::numeric_limits<double>::quiet_NaN();
    double climbMs = std::numeric_limits<double>::quiet_NaN();
    double headingDeg = std::numeric_limits<double>::quiet_NaN();
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::PositionBundle)
