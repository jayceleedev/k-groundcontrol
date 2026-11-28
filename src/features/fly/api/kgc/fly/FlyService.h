// [0.6.0 5.11.2 5.11.7] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/fly/FlyTypes.h>
#include <kgc/app/errors/Result.h>
#include <kgc/vehicle/registry/Vehicle.h>
#include <kgc/vehicle/safety/ArmedPolicy.h>
namespace kgc::params { class ParamService; }
namespace kgc::fly {
class FlyRunner;
class GoToTracker;
/** 기체 하나의 비행 단추 동작(모드·시동·이륙·착륙·RTL·일시정지·비상 정지·여기로 가기)을 판정하고 보내요.
 * 입력: 부모인 Vehicle의 상태·플러그인·명령 큐, 시동 판정 ArmedPolicy, 같은 기체의 ParamService(파라미터 값 읽기)예요.
 * 출력: 지원하는 동작(capabilities)과 지금 누를 수 있는 동작(readiness), 여기로 가기 목표와 선회 원, 동작 결과 신호예요.
 * 실패: 보내기 전 검사 실패와 기체 쪽 실패는 actionFinished의 FlyError로, 여기로 가기 검사는 Result로 알려요.
 */
class FlyService : public QObject {
    Q_OBJECT
public:
/** 상태 신호와 파라미터 묶음을 잇고 실행기와 목표 추적기를 만들어요.
 * 입력: 부모가 될 vehicle, 앱 수명 동안 있는 policy, parent(Composition이 vehicle을 넘겨요)예요.
 * 출력: 지금 상태로 능력·준비를 계산해 둔 서비스예요. 같은 vehicleAdded에서 먼저 생긴 ParamService를 찾아 들어요.
 * 실패: ParamService가 없으면 파라미터 값은 늘 모름이에요.
 */
    FlyService(kgc::vehicle::registry::Vehicle& vehicle, kgc::vehicle::safety::ArmedPolicy& policy, QObject* parent);
/** 아무것도 보내지 않고 사라져요.
 * 입력: 없어요.
 * 출력: 없어요. 정책도 부르지 않아요(기체가 정책보다 늦게 지워질 수 있어요).
 * 실패: 없어요.
 */
    ~FlyService() override;
/** 기체의 비행 서비스를 찾아요.
 * 입력: 기체 vehicle이에요.
 * 출력: 기체의 직접 자식인 FlyService이며, 소유권은 넘기지 않아요.
 * 실패: 없거나 vehicle이 nullptr이면 nullptr예요.
 */
    static FlyService* of(kgc::vehicle::registry::Vehicle* vehicle);
/** 이 서비스의 기체를 읽어요. 입력: 없어요. 출력: 부모 Vehicle이에요. 실패: 없어요. */
    kgc::vehicle::registry::Vehicle& vehicle() const;
/** 지원하는 비행 동작을 지금 상태로 계산해요.
 * 입력: 없어요(HEARTBEAT type, 비행 상태의 vtolState, 펌웨어 버전, 플러그인, 파라미터 값을 읽어요).
 * 출력: 기체 종류·모양·종류를 정했는지·이륙 방식·지원 동작·모드 목록이에요.
 * 실패: 없어요.
 */
    FlyCapabilities capabilities() const;
/** 지금 누를 수 있는 동작을 계산해요.
 * 입력: 없어요(링크·시동·착륙 상태·위치·홈·바쁨·모드를 읽어요).
 * 출력: readinessOf의 결과예요.
 * 실패: 없어요.
 */
    FlyReadiness readiness() const;
/** 비행 명령을 보내고 응답·확인을 기다리는 중인지 읽어요. 입력: 없어요. 출력: 기다리면 참이에요. 실패: 없어요. */
    bool busy() const;
/** 시동을 걸어요. 입력: 없어요. 출력: 결과는 actionFinished(Arm)로 와요. 실패: 보내기 전 검사 실패도 actionFinished로 큐를 거쳐 와요. */
    void arm();
/** 땅에서 시동을 꺼요.
 * 입력: 없어요.
 * 출력: 결과는 actionFinished(Disarm)로 와요. 다른 비행 명령이 진행 중이어도 끼어들어 바로 나가요.
 * 실패: 보내기 전 검사 실패도 actionFinished로 큐를 거쳐 와요.
 * 흐름:
 * 1. readiness().disarm과 ArmedPolicy::check(FlyAction)를 봐요.
 * 2. FirmwarePlugin::disarmSteps — COMPONENT_ARM_DISARM(param1 0) 단계를 받아요.
 * 3. FlyRunner::start(preempt) — 진행 중 실행을 Preempted로 끝내고 CommandQueue에 끼어들어 보내요.
 * 4. ACK가 오면(받아들임이든 거부든) 3초(kConfirmTimeout) 안에 HEARTBEAT 시동 상태가 꺼지면 성공이고,
 *    아니면 받아들였을 때는 ArmNotChanged, 거부였을 때는 그 ACK의 오류로 실패예요.
 * 5. ACK가 끝내 오지 않으면 큐가 재시도 뒤 포기한 때(Timeout, 약 4.8초) HEARTBEAT가 이미 꺼짐이면 성공, 아니면 NoAck예요.
 *    링크 없음(NoLink)·기체 사라짐(Gone)은 곧바로 실패예요.
 */
    void disarm();
/** 공중에서도 모터를 바로 꺼요(param2 21196).
 * 입력: 없어요.
 * 출력: 결과는 actionFinished(EmergencyStop)로 와요. 다른 비행 명령이 진행 중이어도 끼어들어 바로 나가요.
 * 실패: 보내기 전 검사 실패도 actionFinished로 큐를 거쳐 와요.
 * 흐름:
 * 1. readiness().emergencyStop과 ArmedPolicy::check(FlyAction)를 봐요.
 * 2. FirmwarePlugin::emergencyStopSteps — COMPONENT_ARM_DISARM(param1 0, param2 21196) 단계를 받아요.
 * 3. FlyRunner::start(preempt) — 진행 중 실행을 Preempted로 끝내고 CommandQueue에 끼어들어 보내요.
 * 4. ACK가 오면(받아들임이든 거부든) 3초(kConfirmTimeout) 안에 HEARTBEAT 시동 상태가 꺼지면 성공이고,
 *    아니면 받아들였을 때는 ArmNotChanged, 거부였을 때는 그 ACK의 오류로 실패예요.
 * 5. ACK가 끝내 오지 않으면 큐가 재시도 뒤 포기한 때(Timeout, 약 4.8초) HEARTBEAT가 이미 꺼짐이면 성공, 아니면 NoAck예요.
 *    링크 없음(NoLink)·기체 사라짐(Gone)은 곧바로 실패예요.
 */
    void emergencyStop();
/** 지금 자리 위 높이로 이륙해요.
 * 입력: 지금 자리 위 높이 heightM이에요. FixedAltitude 이륙이면 무시해요(NaN 가능).
 * 출력: 결과는 actionFinished(Takeoff)로 와요.
 * 실패: 고도 한계 밖은 AltitudeOutOfRange, 기체 고도를 모르면 PositionUnknown이며 actionFinished로 큐를 거쳐 와요.
 * 흐름:
 * 1. capabilities().takeoff로 이륙 방식을 봐요.
 * 2. WithAltitude면 checkGuided로 높이를 보고, heightIn(takeoffAltitudeFrame)으로 기체가 읽는 값으로 바꿔요.
 * 3. FirmwarePlugin::takeoffSteps — 모드·시동·NAV_TAKEOFF 단계를 받아요(이미 시동이면 시동 단계는 빠져요).
 * 4. start — readiness().takeoff를 다시 보고(거짓이면 NotReady), 다른 비행 명령이 진행 중이면 Busy,
 *    ArmedPolicy::check(FlyAction)가 Deny면 PolicyDenied, 단계가 비면 Unsupported예요. 실패는 actionFinished로 큐를 거쳐 와요.
 * 5. FlyRunner — 단계마다 ACK와 모드·시동 확인을 기다리고 다음 단계를 보내요.
 */
    void takeoff(double heightM);
/** 착륙을 시작해요. 입력: 없어요. 출력: 결과는 actionFinished(Land)로 와요. 실패: 보내기 전 검사 실패도 actionFinished로 와요. */
    void land();
/** RTL을 시작해요. 입력: 없어요. 출력: 결과는 actionFinished(Rtl)로 와요. 실패: 보내기 전 검사 실패도 actionFinished로 와요. */
    void rtl();
/** 그 자리에 멈춰요(일시정지). 입력: 없어요. 출력: 결과는 actionFinished(Pause)로 와요. 실패: 보내기 전 검사 실패도 actionFinished로 와요. */
    void pause();
/** 비행 모드를 바꿔요.
 * 입력: 플러그인 모드 목록의 customMode예요.
 * 출력: 결과는 actionFinished(SetMode)로 와요.
 * 실패: 목록에 없거나 지금 고를 수 없는 모드(공중의 스틱 모드)는 NotReady로 큐를 거쳐 와요.
 */
    void setMode(std::uint32_t customMode);
/** 여기로 가기를 보낼 수 있는지 검사해요(보내지 않아요).
 * 입력: 목표 위도 latDeg·경도 lonDeg, 홈 기준 고도 altitudeRelM이에요.
 * 출력: 보낼 수 있으면 성공이에요.
 * 실패: 순서대로 NotReady(지금 못 함·모르는 좌표), TooFar, AltitudeOutOfRange, HomeUnknown이에요.
 */
    kgc::app::errors::Result<void> checkGoTo(double latDeg, double lonDeg, double altitudeRelM) const;
/** 기체를 고른 곳과 홈 기준 고도로 보내요.
 * 입력: 목표 위도 latDeg·경도 lonDeg, 홈 기준 고도 altitudeRelM이에요.
 * 출력: 검사를 지나면 성공이고, 결과는 actionFinished(GoTo)로, 목표 단계는 target·targetChanged로 와요.
 * 실패: checkGoTo의 오류를 바로 돌려주며 그때는 actionFinished가 없어요.
 * 흐름:
 * 1. checkGoTo — 준비·좌표·거리·고도 한계·홈 고도를 봐요.
 * 2. altitudeIn(goToAltitudeFrame)으로 기체가 읽는 고도로 바꾸고 FirmwarePlugin::goToSteps로 DO_REPOSITION 단계를 받아요.
 * 3. GoToTracker::begin — 목표를 Sending(점선 표식)으로 두고, 받아들여졌던 앞 목표는 기억해요.
 * 4. start — readiness().goTo를 다시 보고(거짓이면 NotReady), 다른 비행 명령이 진행 중이면 Busy,
 *    ArmedPolicy::check(FlyAction)가 Deny면 PolicyDenied, 단계가 비면 Unsupported예요. 실패는 actionFinished로 큐를 거쳐 오고,
 *    GoToTracker::fail이 앞 목표가 아직 살아 있으면(시동 중이고 그 모드) 되살리고 아니면 목표를 지워요.
 * 5. FlyRunner — COMMAND_INT를 보내고 ACK와 유도 모드 확인을 기다려요. ModeNotChanged가 아닌 실패면 GoToTracker::fail이
 *    같은 규칙으로 앞 목표를 되살리거나 지워요.
 * 6. 성공이면 GoToTracker::accept — 5초 안 기체 목표(POSITION_TARGET_GLOBAL_INT)가 10 m 안인지 봐요. 먼 목표만 보이면
 *    TargetRejected를 알리고, 기체 목표가 앞 목표 자리면 앞 목표를 되살리고 아니면 지워요.
 *    모드가 안 바뀌었으면(ModeNotChanged) GoToTracker::watch로 5초 더 기체 목표를 보고 그 뒤에 결과를 알려요.
 */
    kgc::app::errors::Result<void> goTo(double latDeg, double lonDeg, double altitudeRelM);
/** 기체 지금 위치에서 점까지의 수평 거리를 재요. 입력: 위도·경도예요. 출력: 미터예요. 실패: 기체 위치를 모르면 NaN이에요. */
    double distanceToM(double latDeg, double lonDeg) const;
/** 지금 여기로 가기 목표를 읽어요. 입력: 없어요. 출력: GoToTarget이며, 없으면 None이에요. 실패: 없어요. */
    GoToTarget target() const;
/** 그릴 선회 원을 계산해요.
 * 입력: 없어요(목표 단계, 기체 종류·모양, ORBIT 반지름, 선회 반지름·떠 있기 파라미터를 읽어요).
 * 출력: 목표가 Accepted·Confirmed일 때 플러그인 loiterCircle의 결과, 아니면 빈 값이에요.
 * 실패: 없어요.
 */
    kgc::vehicle::firmware::LoiterCircle loiter() const;
signals:
/** capabilities()가 바뀌었음을 알려요. 입력: 없어요. 출력: 다시 읽도록 알려요. 실패: 없어요. */
    void capabilitiesChanged();
/** readiness()나 busy()가 바뀌었음을 알려요. 입력: 없어요. 출력: 다시 읽도록 알려요. 실패: 없어요. */
    void readinessChanged();
/** target()이 바뀌었음을 알려요. 입력: 없어요. 출력: 다시 읽도록 알려요. 실패: 없어요. */
    void targetChanged();
/** loiter()가 바뀌었음을 알려요. 입력: 없어요. 출력: 다시 읽도록 알려요. 실패: 없어요. */
    void loiterChanged();
/** 비행 동작 하나가 끝났음을 알려요.
 * 입력: 동작 종류 kind, 결과 result, 실패를 설명하는 기체 메시지 vehicleReason(심각도 5 이하 STATUSTEXT, 없으면 빈 값)이에요.
 * 출력: 보통 동작마다 한 번이에요. 보내기 전 검사 실패도 큐를 거쳐 한 번 와요. 여기로 가기는 둘이 달라요:
 *       성공을 낸 뒤 5초 안 기체 목표가 다른 곳이면 TargetRejected로 한 번 더 와요. ModeNotChanged로 끝난 여기로 가기는
 *       그때 내지 않고 5초 지켜본 뒤 성공·TargetWithoutMode·ModeNotChanged 중 하나로 한 번 와요.
 * 실패: 없어요.
 */
    void actionFinished(kgc::fly::FlyActionKind kind, kgc::app::errors::Result<void> result, const QString& vehicleReason);
private:
    void refresh();
    std::optional<double> paramValue(const QString& name) const;
    void start(FlyActionKind kind, QList<kgc::vehicle::firmware::CommandStep> steps);
    void finishLater(FlyActionKind kind, kgc::app::errors::ErrorCode error);
    void onRunnerFinished(FlyActionKind kind, kgc::app::errors::Result<void> result, const QString& reason);
    void onWatchEnded(bool kept);
    kgc::vehicle::registry::Vehicle& vehicle_;
    kgc::vehicle::safety::ArmedPolicy& policy_;
    QPointer<kgc::params::ParamService> params_;
    FlyRunner* runner_;
    GoToTracker* tracker_;
    FlyCapabilities capabilities_;
    FlyReadiness readiness_;
    bool busy_ = false;
    kgc::vehicle::firmware::LoiterCircle loiter_;
    QString pendingReason_;
};
}
