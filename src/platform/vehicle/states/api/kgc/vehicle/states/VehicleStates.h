// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/states/LinkBundle.h>
#include <kgc/vehicle/states/ArmingBundle.h>
#include <kgc/vehicle/states/AttitudeBundle.h>
#include <kgc/vehicle/states/PositionBundle.h>
#include <kgc/vehicle/states/GpsBundle.h>
#include <kgc/vehicle/states/BatteryBundle.h>
#include <kgc/vehicle/states/CapabilitiesBundle.h>
namespace kgc::vehicle::states {
class VehicleStateOwner;
/** 기체별 읽기 전용 상태 묶음을 제공해요.
 * 입력: VehicleStateOwner가 반영하는 수신 값과 끊김 판정이에요.
 * 출력: 일곱 상태 묶음의 복사본과 각각의 변경 시그널이에요.
 * 실패: 없어요.
 */
class VehicleStates : public QObject {
    Q_OBJECT
    Q_PROPERTY(LinkBundle link READ link NOTIFY linkChanged)
    Q_PROPERTY(ArmingBundle arming READ arming NOTIFY armingChanged)
    Q_PROPERTY(AttitudeBundle attitude READ attitude NOTIFY attitudeChanged)
    Q_PROPERTY(PositionBundle position READ position NOTIFY positionChanged)
    Q_PROPERTY(GpsBundle gps READ gps NOTIFY gpsChanged)
    Q_PROPERTY(BatteryBundle battery READ battery NOTIFY batteryChanged)
    Q_PROPERTY(CapabilitiesBundle capabilities READ capabilities NOTIFY capabilitiesChanged)
public:
/** 알 수 없는 값으로 상태를 시작해요.
 * 입력: 수명을 함께할 부모 객체이며, 없으면 호출자가 소유해요.
 * 출력: 수신 전 기본값을 가진 일곱 상태 묶음이에요.
 * 실패: 없어요.
 */
    explicit VehicleStates(QObject* parent = nullptr) : QObject(parent) {}
/** Link 묶음의 마지막 값을 읽어요.
 * 입력: 없어요.
 * 출력: 소유자가 갱신한 LinkBundle 복사본이며, 아직 받지 않았으면 묶음의 기본값이에요.
 * 실패: 없어요.
 */
    LinkBundle link() const { return link_; }
/** Arming 묶음의 마지막 값을 읽어요.
 * 입력: 없어요.
 * 출력: 소유자가 갱신한 ArmingBundle 복사본이며, 아직 받지 않았으면 묶음의 기본값이에요.
 * 실패: 없어요.
 */
    ArmingBundle arming() const { return arming_; }
/** Attitude 묶음의 마지막 값을 읽어요.
 * 입력: 없어요.
 * 출력: 소유자가 갱신한 AttitudeBundle 복사본이며, 아직 받지 않았으면 묶음의 기본값이에요.
 * 실패: 없어요.
 */
    AttitudeBundle attitude() const { return attitude_; }
/** Position 묶음의 마지막 값을 읽어요.
 * 입력: 없어요.
 * 출력: 소유자가 갱신한 PositionBundle 복사본이며, 아직 받지 않았으면 묶음의 기본값이에요.
 * 실패: 없어요.
 */
    PositionBundle position() const { return position_; }
/** Gps 묶음의 마지막 값을 읽어요.
 * 입력: 없어요.
 * 출력: 소유자가 갱신한 GpsBundle 복사본이며, 아직 받지 않았으면 묶음의 기본값이에요.
 * 실패: 없어요.
 */
    GpsBundle gps() const { return gps_; }
/** Battery 묶음의 마지막 값을 읽어요.
 * 입력: 없어요.
 * 출력: 소유자가 갱신한 BatteryBundle 복사본이며, 아직 받지 않았으면 묶음의 기본값이에요.
 * 실패: 없어요.
 */
    BatteryBundle battery() const { return battery_; }
/** Capabilities 묶음의 마지막 값을 읽어요.
 * 입력: 없어요.
 * 출력: 소유자가 갱신한 CapabilitiesBundle 복사본이며, 아직 받지 않았으면 묶음의 기본값이에요.
 * 실패: 없어요.
 */
    CapabilitiesBundle capabilities() const { return capabilities_; }
signals:
/** Link 묶음을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: link 속성의 갱신 알림이며, 새 값은 해당 접근자로 읽어요.
 * 실패: 없어요.
 */
    void linkChanged();
/** Arming 묶음을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: arming 속성의 갱신 알림이며, 새 값은 해당 접근자로 읽어요.
 * 실패: 없어요.
 */
    void armingChanged();
/** Attitude 묶음을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: attitude 속성의 갱신 알림이며, 새 값은 해당 접근자로 읽어요.
 * 실패: 없어요.
 */
    void attitudeChanged();
/** Position 묶음을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: position 속성의 갱신 알림이며, 새 값은 해당 접근자로 읽어요.
 * 실패: 없어요.
 */
    void positionChanged();
/** Gps 묶음을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: gps 속성의 갱신 알림이며, 새 값은 해당 접근자로 읽어요.
 * 실패: 없어요.
 */
    void gpsChanged();
/** Battery 묶음을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: battery 속성의 갱신 알림이며, 새 값은 해당 접근자로 읽어요.
 * 실패: 없어요.
 */
    void batteryChanged();
/** Capabilities 묶음을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: capabilities 속성의 갱신 알림이며, 새 값은 해당 접근자로 읽어요.
 * 실패: 없어요.
 */
    void capabilitiesChanged();
private:
    friend class VehicleStateOwner;
    LinkBundle link_;
    ArmingBundle arming_;
    AttitudeBundle attitude_;
    PositionBundle position_;
    GpsBundle gps_;
    BatteryBundle battery_;
    CapabilitiesBundle capabilities_;
};
}
