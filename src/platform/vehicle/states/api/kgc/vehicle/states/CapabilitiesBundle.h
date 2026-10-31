// [0.2.0 T11] specs/features/0.2.0-fcflash.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QObject>
#include <QString>
#include <limits>
namespace kgc::vehicle::states {
/** 하트비트에서 확인한 기체 종류와 펌웨어 이름, AUTOPILOT_VERSION의 펌웨어 버전을 전달해요.
 * 입력: HEARTBEAT의 기체 종류, 플러그인의 표시 이름, AUTOPILOT_VERSION의 비행 소프트웨어 버전이에요.
 * 출력: vehicleType·firmwareName·firmwareVersion이며, 수신 전은 -1·빈 문자열·빈 문자열이에요.
 * 실패: 없어요.
 */
struct CapabilitiesBundle {
    Q_GADGET
    Q_PROPERTY(int vehicleType MEMBER vehicleType)
    Q_PROPERTY(QString firmwareName MEMBER firmwareName)
    Q_PROPERTY(QString firmwareVersion MEMBER firmwareVersion)
public:
    int vehicleType = -1;
    QString firmwareName = {};
    QString firmwareVersion = {};
};
}
Q_DECLARE_METATYPE(kgc::vehicle::states::CapabilitiesBundle)
