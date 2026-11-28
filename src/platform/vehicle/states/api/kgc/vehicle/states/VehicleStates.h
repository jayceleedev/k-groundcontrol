// [0.6.0 5.4] specs/features/0.6.0-fly.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/states/LinkBundle.h>
#include <kgc/vehicle/states/ArmingBundle.h>
#include <kgc/vehicle/states/AttitudeBundle.h>
#include <kgc/vehicle/states/PositionBundle.h>
#include <kgc/vehicle/states/GpsBundle.h>
#include <kgc/vehicle/states/BatteryBundle.h>
#include <kgc/vehicle/states/CapabilitiesBundle.h>
#include <kgc/vehicle/states/FlightBundle.h>
#include <kgc/vehicle/states/NavTargetBundle.h>
#include <kgc/vehicle/states/VehicleMessage.h>
namespace kgc::vehicle::states {
class VehicleStateOwner;
/** 기체별 읽기 전용 상태 묶음을 제공해요.
 * 입력: VehicleStateOwner가 반영하는 수신 값과 끊김·만료 판정이에요.
 * 출력: 아홉 상태 묶음과 기체 메시지 신호예요. 묶음은 복사본으로 읽고 각각의 변경 시그널이 있어요.
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
    Q_PROPERTY(FlightBundle flight READ flight NOTIFY flightChanged)
    Q_PROPERTY(NavTargetBundle navTarget READ navTarget NOTIFY navTargetChanged)
public:
/** 알 수 없는 값으로 상태를 시작해요.
 * 입력: 수명을 함께할 부모 객체이며, 없으면 호출자가 소유해요.
 * 출력: 수신 전 기본값을 가진 아홉 상태 묶음이에요.
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
/** Flight 묶음(착륙 상태·VTOL 상태)의 마지막 값을 읽어요.
 * 입력: 없어요.
 * 출력: 소유자가 갱신한 FlightBundle 복사본이에요. 받기 전이나 5초 동안 오지 않았거나 링크가 끊겼으면 둘 다 -1이에요.
 * 실패: 없어요.
 */
    FlightBundle flight() const { return flight_; }
/** NavTarget 묶음(기체 항법 목표와 선회)의 마지막 값을 읽어요.
 * 입력: 없어요.
 * 출력: 소유자가 갱신한 NavTargetBundle 복사본이에요. 3초 안 온 목표·선회는 NaN이고 targetCount는 받은 목표 메시지 수예요.
 * 실패: 없어요.
 */
    NavTargetBundle navTarget() const { return navTarget_; }
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
/** Flight 묶음을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: flight 속성의 갱신 알림이며, 새 값은 해당 접근자로 읽어요.
 * 실패: 없어요.
 */
    void flightChanged();
/** NavTarget 묶음을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: navTarget 속성의 갱신 알림이며, 새 값은 해당 접근자로 읽어요. 만료로 비울 때도 한 번 알려요.
 * 실패: 없어요.
 */
    void navTargetChanged();
/** 기체가 보낸 STATUSTEXT 하나를 알려요.
 * 입력: 받은 기체 메시지 message(심각도 severity와 글 text)예요.
 * 출력: STATUSTEXT 하나마다 한 번 나와요(직접 연결). 목록은 두지 않아요. 나뉜 긴 글은 조각마다 따로 내요.
 * 실패: 없어요.
 */
    void vehicleMessage(kgc::vehicle::states::VehicleMessage message);
private:
    friend class VehicleStateOwner;
    LinkBundle link_;
    ArmingBundle arming_;
    AttitudeBundle attitude_;
    PositionBundle position_;
    GpsBundle gps_;
    BatteryBundle battery_;
    CapabilitiesBundle capabilities_;
    FlightBundle flight_;
    NavTargetBundle navTarget_;
};
}
