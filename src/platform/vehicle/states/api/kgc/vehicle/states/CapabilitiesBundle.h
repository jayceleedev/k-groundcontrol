// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QObject>
#include <QString>
#include <limits>
namespace kgc::vehicle::states {
/** 하트비트에서 확인한 기체 종류와 펌웨어 이름을 전달해요.
 * 입력: HEARTBEAT의 기체 종류와 플러그인의 표시 이름이에요.
 * 출력: vehicleType·firmwareName이며, 수신 전은 -1·빈 문자열이에요.
 * 실패: 없어요.
 */
struct CapabilitiesBundle {
    Q_GADGET
    Q_PROPERTY(int vehicleType MEMBER vehicleType)
    Q_PROPERTY(QString firmwareName MEMBER firmwareName)
public:
    int vehicleType = -1;
    QString firmwareName = {};
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::CapabilitiesBundle)
