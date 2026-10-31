// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QObject>
#include <QString>
#include <limits>
namespace kgc::vehicle::states {
/** 위치 고정 수준과 위성 정보를 전달해요.
 * 입력: GPS_RAW_INT의 fix_type·satellites_visible·eph에서 변환한 값이에요.
 * 출력: fixType·satellites·hdop이며, 수신 전 정수는 -1이고 hdop는 NaN이에요.
 * 실패: 없어요.
 */
struct GpsBundle {
    Q_GADGET
    Q_PROPERTY(int fixType MEMBER fixType)
    Q_PROPERTY(int satellites MEMBER satellites)
    Q_PROPERTY(double hdop MEMBER hdop)
public:
    int fixType = -1;
    int satellites = -1;
    double hdop = std::numeric_limits<double>::quiet_NaN();
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::GpsBundle)
