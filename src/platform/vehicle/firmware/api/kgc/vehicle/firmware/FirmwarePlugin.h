// [0.6.0 §12 1] specs/features/0.6.0-fly.md
// [0.6.0 5.5] specs/features/0.6.0-fly.md
// [0.4.2 5.1] specs/features/0.4.2-param-tree.md
// [0.3.0 5.2] specs/features/0.3.0-params.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/app/geo/Altitude.h>
#include <QString>
#include <QStringList>
#include <QList>
#include <QUrl>
#include <cstdint>
#include <optional>
namespace kgc::vehicle::firmware {
/** 요청할 스트림 번호와 필요한 수신 빈도를 전달해요.
 * 입력: 스트림 식별자 streamId와 초당 횟수 rateHz예요.
 * 출력: 송신 문이 REQUEST_DATA_STREAM을 만들 때 사용할 두 값이에요.
 * 실패: 없어요.
 */
struct StreamRequest { std::uint8_t streamId; std::uint16_t rateHz; };
/** 파라미터 값을 PARAM_VALUE의 float 칸에 담는 방식을 나타내요.
 * 입력: 없어요.
 * 출력: CCast는 숫자를 float로 옮기고, Bytewise는 정수의 비트를 float 칸에 그대로 담아요.
 * 실패: 없어요.
 */
enum class ParamEncoding { CCast, Bytewise };
/** 파라미터 메타데이터 파일의 모양을 나타내요.
 * 입력: 없어요.
 * 출력: None은 출처 없음, Flat은 PX4 목록형, Grouped는 ArduPilot 묶음형이에요.
 * 실패: 없어요.
 */
enum class ParamMetadataFormat { None, Flat, Grouped };
/** 파라미터 메타데이터를 어디서 받을지 전달해요.
 * 입력: 파일 모양 format, 차례로 시도할 주소 urls, 캐시 파일 이름의 가운데 cacheName이에요.
 * 출력: 기능 모듈이 내려받을 때 쓰는 값이에요.
 * 실패: 없어요.
 */
struct ParamMetadataSource { ParamMetadataFormat format; QList<QUrl> urls; QString cacheName; };
/** 비행 명령을 정할 때 쓰는 기체 종류예요.
 * 입력: 없어요(vehicleClassOf가 HEARTBEAT type과 vtol_state로 정해요).
 * 출력: Unknown(모름)·Multicopter·FixedWing·Vtol·Other(로버·보트 같은 그 밖)예요.
 * 실패: 없어요.
 */
enum class VehicleClass { Unknown, Multicopter, FixedWing, Vtol, Other };
/** 기체가 지금 나는 모양이에요.
 * 입력: 없어요(flightShapeOf가 정해요).
 * 출력: Hover는 지금 떠 있는 모양, Forward는 앞으로 나는 모양이에요.
 * 실패: 없어요.
 */
enum class FlightShape { Hover, Forward };
/** 이륙을 지원하는 방식이에요.
 * 입력: 없어요.
 * 출력: None은 지원 안 함, WithAltitude는 높이를 넣어 이륙, FixedAltitude는 기체 파라미터 고도로 이륙이에요.
 * 실패: 없어요.
 */
enum class TakeoffKind { None, WithAltitude, FixedAltitude };
/** 명령 단계를 어느 메시지로 보낼지예요.
 * 입력: 없어요.
 * 출력: Long은 COMMAND_LONG, Int는 COMMAND_INT예요.
 * 실패: 없어요.
 */
enum class CommandForm { Long, Int };
/** BASE §8의 명령 단계 하나예요. 기능 모듈이 차례로 보내고 확인해요.
 * 입력: 보낼 방식 form, 명령 번호 command, param1~7(Long만 5~7), frame·x·y·z(Int만), 확인 조건 confirmModes·confirmArmed예요.
 * 출력: confirmModes가 비지 않으면 ACK 뒤 HEARTBEAT custom_mode가 이 중 하나가 되어야 끝, confirmArmed가 있으면 ACK 뒤 시동 상태가 이 값이 되어야 끝이에요.
 *       NaN 칸은 ==로 비교하면 거짓이라 시험은 칸마다 isnan으로 봐요.
 * 실패: 없어요.
 */
struct CommandStep {
    CommandForm form = CommandForm::Long;
    std::uint16_t command = 0;
    float param1 = 0.0F, param2 = 0.0F, param3 = 0.0F, param4 = 0.0F;
    float param5 = 0.0F, param6 = 0.0F, param7 = 0.0F; // Long만
    std::uint8_t frame = 0;                            // Int만
    std::int32_t x = 0, y = 0;                         // Int만
    float z = 0.0F;                                    // Int만
    QList<std::uint32_t> confirmModes;                 // 비지 않으면 ACK 뒤 HEARTBEAT custom_mode가 이 중 하나가 되어야 끝
    std::optional<bool> confirmArmed;                  // 있으면 ACK 뒤 시동 상태가 이 값이 되어야 끝
    bool operator==(const CommandStep&) const = default;
};
/** 모드 내림 목록의 항목 하나예요.
 * 입력: 없어요.
 * 출력: 사용자 모드 번호 customMode, modeName과 같은 공식 이름 name, 조종기 스틱을 따르는 모드인지 pilotControlled(공중에서 흐리게)예요.
 * 실패: 없어요.
 */
struct FlightMode {
    std::uint32_t customMode = 0;
    QString name;
    bool pilotControlled = false;                      // 조종기 스틱을 따르는 모드(공중에서 흐리게, 결정 5)
    bool operator==(const FlightMode&) const = default;
};
/** 연결 때 SET_MESSAGE_INTERVAL로 요청할 메시지 하나예요.
 * 입력: 없어요.
 * 출력: 메시지 번호 messageId와 간격 intervalUs(µs)예요.
 * 실패: 없어요.
 */
struct MessageInterval { std::uint16_t messageId = 0; std::int32_t intervalUs = 0; };
/** 지도에 그릴 선회 원이에요.
 * 입력: 없어요.
 * 출력: 그릴지 show, 반지름 radiusM(m, 양수), 도는 방향 clockwise예요.
 * 실패: 없어요.
 */
struct LoiterCircle { bool show = false; double radiusM = 0.0; bool clockwise = true; bool operator==(const LoiterCircle&) const = default; };
/** HEARTBEAT type과 vtol_state로 기체 종류를 정해요(MAVLink 공통 규칙, 펌웨어를 보지 않아요).
 * 입력: HEARTBEAT type mavType(모르면 음수)과 EXTENDED_SYS_STATE vtol_state vtolState(모르면 -1)예요.
 * 출력: type이 고정익이고 vtolState가 1~4면 Vtol(쿼드플레인), 고정익 FixedWing, VTOL 종류 Vtol, 멀티콥터 종류 Multicopter
 *       (vtolState와 상관없이 — ArduCopter는 늘 3을 보내요), GENERIC·음수 Unknown, 그 밖 Other예요(QGC와 같은 규칙).
 * 실패: 없어요.
 */
VehicleClass vehicleClassOf(int mavType, int vtolState);
/** 기체가 지금 나는 모양을 정해요.
 * 입력: 기체 종류 vehicleClass와 vtol_state vtolState예요.
 * 출력: FixedWing은 Forward, Vtol은 vtolState가 4(고정익)나 1(고정익으로 전환 중)이면 Forward 아니면 Hover, 그 밖은 Hover예요.
 * 실패: 없어요.
 */
FlightShape flightShapeOf(VehicleClass vehicleClass, int vtolState);
/** 펌웨어별 표시와 스트림 요청, 파라미터 규칙, 비행 명령 데이터를 제공해요.
 * 입력: 모드 조회의 customMode·mavType과 첫 단계 가지의 접두어·기체 종류·버전, 비행 명령의 기체 종류·모양·값이에요.
 * 출력: 공식 펌웨어·모드 이름, 초기 스트림·메시지 주기 요청 목록, 파라미터 값 방식·메타데이터 출처·묶음 제목, 모드 목록과 명령 단계(CommandStep) 목록이에요.
 *       플러그인은 보내지 않고 상태를 쓰지 않아요. 빈 단계 목록은 지원 안 함(단추 숨김)이에요.
 * 실패: 모르는 모드 이름은 빈 문자열로 돌려줘요.
 */
class FirmwarePlugin {
public:
/** 플러그인 자원을 정리해요.
 * 입력: 없어요.
 * 출력: 기체가 소유하던 플러그인의 자원을 해제해요.
 * 실패: 없어요.
 */
    virtual ~FirmwarePlugin() = default;
/** 공식 펌웨어 이름을 돌려줘요.
 * 입력: 없어요.
 * 출력: PX4·ArduPilot 또는 범용 플러그인의 MAVLink 이름이에요.
 * 실패: 없어요.
 */
    virtual QString displayName() const = 0;
/** 모드 번호를 공식 이름으로 바꿔요.
 * 입력: 사용자 모드 번호 customMode와 기체 종류 mavType이에요.
 * 출력: 알려진 모드의 공식 이름이며, 모르는 조합은 빈 문자열이에요.
 * 실패: 없어요.
 */
    virtual QString modeName(std::uint32_t customMode, std::uint8_t mavType) const = 0;
/** 연결할 때 요청할 스트림 목록을 돌려줘요.
 * 입력: 없어요.
 * 출력: ArduPilot의 상태·위치·자세 요청 목록이며, PX4와 범용 플러그인은 빈 목록이에요.
 * 실패: 없어요.
 */
    virtual QList<StreamRequest> streamRequests() const = 0;
/** 파라미터 값을 float 칸에 담는 방식을 돌려줘요.
 * 입력: 없어요.
 * 출력: PX4는 Bytewise, ArduPilot과 범용 플러그인은 CCast예요.
 * 실패: 없어요.
 */
    virtual ParamEncoding paramEncoding() const = 0;
/** 파라미터 메타데이터의 출처를 돌려줘요.
 * 입력: HEARTBEAT의 기체 종류 mavType과 major.minor.patch 꼴의 version이에요.
 * 출력: 주소와 캐시 이름이에요. 출처가 없으면 format이 None이에요.
 * 실패: 모르는 기체 종류나 읽을 수 없는 버전은 None이에요.
 */
    virtual ParamMetadataSource paramMetadataSource(int mavType, const QString& version) const = 0;
/** 파라미터 트리 첫 단계 가지의 사람이 읽는 묶음 제목을 돌려줘요.
 * 입력: 첫 단계 가지의 접두어 그대로인 prefix예요(BATT, BATT2, EK3, SERVO5).
 * 출력: 현재 언어의 제목이며, 제목이 없으면 빈 문자열이에요. ArduPilot은 끝의 숫자를 모두 뗀 접두어로 표에서 찾아요.
 * 실패: 없어요.
 */
    virtual QString paramGroupTitle(const QString& prefix) const = 0;
/** 파일 비교에서 건너뛸 파라미터 이름을 돌려줘요.
 * 입력: 없어요.
 * 출력: 기체 고유 값이나 계수처럼 파일에서 올리면 안 되는 이름 목록이에요.
 * 실패: 없어요.
 */
    virtual QStringList paramFileSkip() const = 0;
/** 연결 때 SET_MESSAGE_INTERVAL로 요청할 메시지를 돌려줘요.
 * 입력: 없어요.
 * 출력: ArduPilot은 EXTENDED_SYS_STATE 1 Hz, PX4와 범용 플러그인은 빈 목록이에요(PX4는 스스로 보내요).
 * 실패: 없어요.
 */
    virtual QList<MessageInterval> messageIntervals() const = 0;
/** 이 HEARTBEAT type이면 vtol_state를 받기 전에는 기체 종류를 정할 수 없는지 알려요.
 * 입력: HEARTBEAT type mavType이에요.
 * 출력: 참이면 비행 상태를 받기 전 종류를 모르는 것으로 봐요(ArduPilot 고정익·VTOL 19·20·21, PX4 VTOL 종류, 범용 고정익).
 * 실패: 없어요.
 */
    virtual bool vtolStateDecidesClass(int mavType) const = 0;
/** 시동을 걸면 곧바로 이륙하는 모드를 돌려줘요.
 * 입력: 기체 종류 vehicleClass예요.
 * 출력: 사용자 모드 번호 목록이에요. 이 모드에서는 '시동' 단추가 흐려요. 없으면 빈 목록이에요.
 * 실패: 없어요.
 */
    virtual QList<std::uint32_t> launchOnArmModes(VehicleClass vehicleClass) const = 0;
/** 모드 내림 목록을 돌려줘요.
 * 입력: 기체 종류 vehicleClass예요.
 * 출력: 화면에 보일 차례 그대로의 모드 목록이에요. 이름은 modeName과 같은 글자이고 스틱을 따르는 모드는 pilotControlled가 참이에요.
 * 실패: 모드를 모르는 종류는 빈 목록이에요.
 */
    virtual QList<FlightMode> flightModes(VehicleClass vehicleClass) const = 0;
/** 여기로 가기 목표가 살아 있는 모드를 돌려줘요.
 * 입력: 기체 종류 vehicleClass예요.
 * 출력: 사용자 모드 번호예요.
 * 실패: 여기로 가기를 지원하지 않는 종류는 값이 없어요.
 */
    virtual std::optional<std::uint32_t> guidedMode(VehicleClass vehicleClass) const = 0;
/** 모드 바꾸기 단계를 돌려줘요.
 * 입력: 기체 종류 vehicleClass(확인할 모드 목록을 정하는 데만 써요)와 바꿀 사용자 모드 번호 customMode예요.
 * 출력: DO_SET_MODE 단계 목록이에요. 확인 모드는 바뀐 뒤 기체가 스스로 옮겨 가는 모드까지 담아요(쿼드플레인 RTL → QRTL·QLAND).
 * 실패: 모드를 바꿀 수 없는 펌웨어는 빈 목록이에요.
 */
    virtual QList<CommandStep> setModeSteps(VehicleClass vehicleClass, std::uint32_t customMode) const = 0;
/** 시동 단계를 돌려줘요.
 * 입력: 없어요.
 * 출력: COMPONENT_ARM_DISARM(param1 1) 한 단계이고 시동이 걸린 것을 확인해요.
 * 실패: 없어요.
 */
    virtual QList<CommandStep> armSteps() const = 0;
/** 시동 끄기 단계를 돌려줘요.
 * 입력: 없어요.
 * 출력: COMPONENT_ARM_DISARM(param1 0) 한 단계이고 시동이 꺼진 것을 확인해요.
 * 실패: 없어요.
 */
    virtual QList<CommandStep> disarmSteps() const = 0;
/** 비상 정지 단계를 돌려줘요.
 * 입력: 없어요.
 * 출력: COMPONENT_ARM_DISARM(param1 0, param2 21196) 한 단계이고 시동이 꺼진 것을 확인해요. 공중에서도 모터를 바로 꺼요.
 * 실패: 없어요.
 */
    virtual QList<CommandStep> emergencyStopSteps() const = 0;
/** 이륙 지원과 높이 입력 여부를 돌려줘요.
 * 입력: 기체 종류 vehicleClass예요.
 * 출력: None·WithAltitude·FixedAltitude 중 하나예요.
 * 실패: 없어요.
 */
    virtual TakeoffKind takeoffKind(VehicleClass vehicleClass) const = 0;
/** FixedAltitude 이륙의 고도 파라미터 이름을 돌려줘요.
 * 입력: 없어요.
 * 출력: 파라미터 이름(ArduPilot TKOFF_ALT)이에요.
 * 실패: 없으면 빈 값이에요.
 */
    virtual QString takeoffAltitudeParam() const = 0;
/** 이륙 param7을 기체가 읽는 고도 기준을 돌려줘요.
 * 입력: 기체 종류 vehicleClass예요.
 * 출력: 고도 틀이에요. 기능 모듈이 heightIn으로 입력 높이를 이 틀의 값으로 바꿔요.
 * 실패: 없어요.
 */
    virtual kgc::app::geo::AltitudeFrame takeoffAltitudeFrame(VehicleClass vehicleClass) const = 0;
/** 이륙 단계를 돌려줘요.
 * 입력: 기체 종류 vehicleClass, 이미 takeoffAltitudeFrame(vehicleClass)의 값인 altitude(m, FixedAltitude는 무시), 지금 시동 상태 armed예요.
 * 출력: 모드·시동·NAV_TAKEOFF 단계 목록이에요. 이미 시동이면 시동 단계를 빼요.
 * 실패: 지원하지 않으면 빈 목록이에요.
 */
    virtual QList<CommandStep> takeoffSteps(VehicleClass vehicleClass, double altitude, bool armed) const = 0;
/** 착륙 단계를 돌려줘요.
 * 입력: 기체 종류 vehicleClass예요.
 * 출력: 착륙 모드로 바꾸는 단계 목록이에요.
 * 실패: 지원하지 않으면 빈 목록(단추 숨김)이에요.
 */
    virtual QList<CommandStep> landSteps(VehicleClass vehicleClass) const = 0;
/** RTL 단계를 돌려줘요.
 * 입력: 기체 종류 vehicleClass예요.
 * 출력: 돌아가기 모드로 바꾸는 단계 목록이에요.
 * 실패: 지원하지 않으면 빈 목록이에요.
 */
    virtual QList<CommandStep> rtlSteps(VehicleClass vehicleClass) const = 0;
/** 일시정지 단계를 돌려줘요.
 * 입력: 기체 종류 vehicleClass, 나는 모양 shape, CapabilitiesBundle.firmwareVersion인 firmwareVersion("major.minor.patch" 또는 빈 값)이에요.
 * 출력: 제자리에 멈추는 단계 목록이에요.
 * 실패: 지원하지 않거나 버전이 안 맞으면 빈 목록이에요(PX4 1.12 전·모르는 버전).
 */
    virtual QList<CommandStep> pauseSteps(VehicleClass vehicleClass, FlightShape shape, const QString& firmwareVersion) const = 0;
/** 여기로 가기 고도의 기준을 돌려줘요.
 * 입력: 없어요.
 * 출력: 고도 틀이에요. 기능 모듈이 altitudeIn으로 홈 기준 고도를 이 틀의 값으로 바꿔요.
 * 실패: 없어요.
 */
    virtual kgc::app::geo::AltitudeFrame goToAltitudeFrame() const = 0;
/** 여기로 가기 단계를 돌려줘요.
 * 입력: 기체 종류 vehicleClass, 목표 위도 latDeg·경도 lonDeg(도), 이미 goToAltitudeFrame의 값인 altitude(m)예요.
 * 출력: DO_REPOSITION(COMMAND_INT) 한 단계이고 유도 모드가 된 것을 확인해요.
 * 실패: 지원하지 않으면 빈 목록이에요.
 */
    virtual QList<CommandStep> goToSteps(VehicleClass vehicleClass, double latDeg, double lonDeg, double altitude) const = 0;
/** 선회 반지름 파라미터 이름을 돌려줘요.
 * 입력: 없어요.
 * 출력: 파라미터 이름이에요(ArduPilot WP_LOITER_RAD, PX4 NAV_LOITER_RAD).
 * 실패: 없으면 빈 값이에요.
 */
    virtual QString loiterRadiusParam() const = 0;
/** 도착 뒤 떠 있기 파라미터 이름을 돌려줘요.
 * 입력: 없어요.
 * 출력: 파라미터 이름이에요(ArduPilot Q_GUIDED_MODE).
 * 실패: 없으면 빈 값이에요.
 */
    virtual QString vtolHoverParam() const = 0;
/** 여기로 가기 목표에 도착해서 도는지 알려요(창의 안내 글).
 * 입력: 기체 종류 vehicleClass, 나는 모양 shape, 도착 뒤 떠 있기 파라미터 값 hoverParam(모르면 없음)이에요.
 * 출력: 돌면 참, 떠 있으면 거짓이에요.
 * 실패: 없어요.
 */
    virtual bool loitersAtTarget(VehicleClass vehicleClass, FlightShape shape, std::optional<double> hoverParam) const = 0;
/** 지도에 그릴 선회 원을 정해요.
 * 입력: 기체 종류 vehicleClass, 나는 모양 shape, ORBIT_EXECUTION_STATUS 반지름 orbitRadiusM(부호 있음, 모르면 NaN),
 *       선회 반지름 파라미터 값 radiusParam과 도착 뒤 떠 있기 파라미터 값 hoverParam(모르면 없음)이에요.
 * 출력: 그릴 원이에요. 반지름은 양수이고 방향은 clockwise예요.
 * 실패: 반지름을 모르거나 돌지 않으면 show가 거짓이에요.
 */
    virtual LoiterCircle loiterCircle(VehicleClass vehicleClass, FlightShape shape, double orbitRadiusM,
                                      std::optional<double> radiusParam, std::optional<double> hoverParam) const = 0;
};
}
