// [0.6.0 5.11.2] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <QList>
#include <QString>
#include <cstdint>
#include <limits>
#include <optional>
namespace kgc::fly {
/** 상단바와 여기로 가기 창이 보내는 비행 동작의 종류예요.
 * 입력: FlyService의 동작 함수가 정해요.
 * 출력: actionFinished와 로그·토스트 글이 이 값으로 동작을 가려요.
 * 실패: 없어요.
 */
enum class FlyActionKind { Arm, Disarm, EmergencyStop, Takeoff, Land, Rtl, Pause, SetMode, GoTo };
/** 여기로 가기 목표가 어디까지 확인됐는지 나타내요.
 * 입력: GoToTracker가 정해요.
 * 출력: None 목표 없음, Sending 보냈고 ACK·모드를 기다림(점선), Accepted 받아들여짐, Confirmed 기체 목표가 같은 자리로 보임이에요.
 * 실패: 없어요.
 */
enum class GoToState { None, Sending, Accepted, Confirmed };
/** 보낸 여기로 가기 목표 하나예요(기능 묶음, BASE §5).
 * 입력: FlyService::goTo가 보낸 위도·경도와 홈 기준 고도예요.
 * 출력: 좌표 latDeg·lonDeg, 홈 기준 고도 altitudeRelM, 확인 단계 state예요. 목표가 없으면 NaN과 None이에요.
 * 실패: 없어요.
 */
struct GoToTarget {
    double latDeg = std::numeric_limits<double>::quiet_NaN();
    double lonDeg = std::numeric_limits<double>::quiet_NaN();
    double altitudeRelM = std::numeric_limits<double>::quiet_NaN();
    GoToState state = GoToState::None;
};
/** 활성 기체가 지원하는 비행 동작을 모아요(펌웨어 플러그인과 기체 종류에서).
 * 입력: FlyService::capabilities가 HEARTBEAT type·비행 상태·플러그인·파라미터 값으로 채워요.
 * 출력: 기체 종류와 모양, 종류를 정했는지, 이륙 방식과 고정 이륙 고도, 착륙·RTL·일시정지·여기로 가기 지원, 모드 목록이에요.
 *       NaN을 두지 않아 ==로 바뀜을 가릴 수 있어요.
 * 실패: 없어요.
 */
struct FlyCapabilities {
    kgc::vehicle::firmware::VehicleClass vehicleClass = kgc::vehicle::firmware::VehicleClass::Unknown;
    kgc::vehicle::firmware::FlightShape shape = kgc::vehicle::firmware::FlightShape::Hover;
    bool classKnown = true;                        // 종류를 정했는지 — type만으로 못 정하면 비행 상태를 받은 뒤 참(§5.11.7)
    kgc::vehicle::firmware::TakeoffKind takeoff = kgc::vehicle::firmware::TakeoffKind::None;
    std::optional<double> takeoffFixedAltitudeM;   // FixedAltitude 이륙 고도 파라미터 값, 모르면 없음
    QString takeoffAltitudeParam;                  // 그 이름(문구용), 없으면 빈 값
    bool land = false, rtl = false, pause = false, goTo = false;
    bool loitersAtTarget = false;                  // 도착하면 도는지(창 안내)
    QList<kgc::vehicle::firmware::FlightMode> modes;
    QList<std::uint32_t> launchOnArmModes;         // 시동을 걸면 곧바로 이륙하는 모드(플러그인 값, '시동' 흐림)
    bool operator==(const FlyCapabilities&) const = default;
};
/** 지금 누를 수 있는 비행 단추를 모아요.
 * 입력: readinessOf가 능력·링크·시동·착륙 상태·위치·홈·바쁨·모드로 정해요.
 * 출력: 동작마다 참이면 지금 보낼 수 있어요.
 * 실패: 없어요.
 */
struct FlyReadiness {
    bool arm = false, disarm = false, emergencyStop = false, takeoff = false, land = false,
         rtl = false, pause = false, setMode = false, goTo = false;
    bool operator==(const FlyReadiness&) const = default;
};
}
