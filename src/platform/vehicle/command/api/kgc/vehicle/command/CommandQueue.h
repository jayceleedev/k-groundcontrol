// [0.6.0 5.3 T1] specs/features/0.6.0-fly.md
// [0.2.0 T6] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/constants/mavlink.h>
#include <kgc/app/errors/Result.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/vehicle/outbound/OutboundKind.h>
#include <kgc/vehicle/outbound/VehicleSender.h>
#include <QObject>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
namespace kgc::vehicle::command {
/** 보낼 COMMAND_LONG 하나의 내용이에요.
 * 입력: 명령 번호 command, param1~7, 대상 컴포넌트 targetCompid(기본 1)예요.
 * 출력: CommandQueue::send에 넘기는 값이에요.
 * 실패: 없어요.
 */
struct CommandLong {
    std::uint16_t command = 0;
    float param1 = 0.0F;
    float param2 = 0.0F;
    float param3 = 0.0F;
    float param4 = 0.0F;
    float param5 = 0.0F;
    float param6 = 0.0F;
    float param7 = 0.0F;
    std::uint8_t targetCompid = 1;
};
/** 보낼 COMMAND_INT 하나의 내용이에요.
 * 입력: 명령 번호 command, 좌표 틀 frame(MAV_FRAME 값), param1~4, 위도 x·경도 y(전역 프레임이면 도 × 1e7), 고도 z, 대상 컴포넌트 targetCompid(기본 1)예요.
 * 출력: CommandQueue::send에 넘기는 값이에요. current와 autocontinue는 늘 0으로 나가요.
 * 실패: 없어요.
 */
struct CommandInt {
    std::uint16_t command = 0;
    std::uint8_t frame = 0;     // MAV_FRAME 값
    float param1 = 0.0F;
    float param2 = 0.0F;
    float param3 = 0.0F;
    float param4 = 0.0F;
    std::int32_t x = 0;         // 위도 × 1e7(전역 프레임)
    std::int32_t y = 0;         // 경도 × 1e7
    float z = 0.0F;
    std::uint8_t targetCompid = 1;
};
/** 명령 하나의 ACK 대기 방식과 송신 종류, 끼어들기 여부를 정해요.
 * 입력: 한 번 보낸 뒤 기다릴 timeout, 재시도 횟수 retries, 송신 종류 kind, 끼어들기 preempt예요.
 *       preempt가 참이면 보내고 기다리던 명령(어느 context든)과, 이 명령과 같은 context로 줄 선 명령만 Preempted로 끝내고 이 명령을 곧바로 보내요.
 *       다른 context로 줄 선 명령은 그대로 두어 이 명령 뒤에 차례로 나가요.
 * 출력: 기본값 1200ms, 3회, OutboundKind::Command, 끼어들지 않음이에요. retries가 3이면 많아야 네 번 나가요.
 * 실패: 없어요.
 */
struct CommandOptions {
    std::chrono::milliseconds timeout = kgc::app::constants::kAckTimeout;
    int retries = kgc::app::constants::kAckRetries;
    kgc::vehicle::outbound::OutboundKind kind = kgc::vehicle::outbound::OutboundKind::Command;
    bool preempt = false;
};
/** ACK 없이 끝난 까닭을 나타내요.
 * 입력: 재시도까지 응답이 없거나, 큐가 멈췄거나, 보낼 링크가 없거나, 끼어든 명령에 밀린 상황이에요.
 * 출력: Timeout·Gone·NoLink·Preempted 중 하나예요.
 * 실패: 없어요.
 */
enum class CommandFailure { Timeout, Gone, NoLink, Preempted };
/** 명령 하나의 끝 결과예요.
 * 입력: 큐가 채워요.
 * 출력: ACK를 받았으면 result에 값이 있고, 아니면 failure에 까닭이 있어요.
 * 실패: 없어요.
 */
struct CommandResult {
    std::optional<MAV_RESULT> result;
    std::optional<CommandFailure> failure;
};
/** 기체 하나의 COMMAND_LONG·COMMAND_INT를 한 큐로 차례로 보내고 ACK를 기다려요.
 * 입력: 송신 함수 send, 대상 sysid, Vehicle이 넘기는 COMMAND_ACK예요.
 * 출력: 명령마다 done을 정확히 한 번 불러요. 한 번에 한 명령만 보내고 기다려요.
 * 실패: 응답 없음·큐 멈춤·링크 없음·끼어든 명령에 밀림은 CommandResult의 failure로 알려요.
 */
class CommandQueue : public QObject {
    Q_OBJECT
public:
    using SendFn = std::function<kgc::app::errors::Result<void>(
        kgc::vehicle::outbound::OutboundKind, kgc::vehicle::outbound::VehicleSender::PackFn)>;
/** 비어 있는 큐를 만들어요.
 * 입력: 송신 함수 send, 대상 시스템 번호 sysid, 수명을 함께할 parent예요.
 * 출력: 명령을 기다리는 큐예요.
 * 실패: 없어요.
 */
    CommandQueue(SendFn send, int sysid, QObject* parent = nullptr);
/** 끝나지 않은 명령을 남기고 지워지지 않아요.
 * 입력: 없어요.
 * 출력: 남은 명령이 있으면 stop과 같이 끝내요.
 * 실패: 없어요.
 */
    ~CommandQueue() override;
/** COMMAND_LONG을 줄 세워요.
 * 입력: 명령 command, options, 끝날 때 부를 done, done을 부를 문맥 context예요.
 * 출력: 앞 명령이 끝나면 options.kind로 보내요. 시간 재기는 실제 송신부터이고 재시도마다 confirmation을 1씩 올려요.
 *       options.preempt가 참이면 앞 명령을 기다리지 않고 이 함수 안에서 곧바로 보내요(규칙은 CommandOptions).
 * 실패: 큐가 멈춘 뒤면 preempt와 상관없이 done에 Gone을 넘겨요. context가 지워졌으면 done을 부르지 않아요.
 */
    void send(CommandLong command, CommandOptions options, std::function<void(CommandResult)> done, QObject* context);
/** COMMAND_INT를 줄 세워요.
 * 입력: 명령 command, options, 끝날 때 부를 done, done을 부를 문맥 context예요.
 * 출력: COMMAND_LONG과 같은 큐·같은 규칙으로 options.kind로 보내요. COMMAND_INT에는 confirmation 칸이 없어 재시도는 같은 내용을 다시 보내요.
 * 실패: 큐가 멈춘 뒤면 preempt와 상관없이 done에 Gone을 넘겨요. context가 지워졌으면 done을 부르지 않아요.
 */
    void send(CommandInt command, CommandOptions options, std::function<void(CommandResult)> done, QObject* context);
/** 기체에서 온 COMMAND_ACK를 받아요.
 * 입력: ACK의 명령 번호 command와 결과 result예요. Vehicle::consume이 넘겨요.
 * 출력: 지금 명령(COMMAND_LONG이든 COMMAND_INT든)과 번호가 같으면 끝내고 다음 명령을 보내요. MAV_RESULT_IN_PROGRESS는 시간을 다시 재고 끝으로 보지 않아요.
 * 실패: 번호가 다르면 무시해요.
 */
    void receiveAck(std::uint16_t command, MAV_RESULT result);
/** 큐를 멈추고 남은 명령을 끝내요.
 * 입력: 없어요. Vehicle::stop이 sender를 지우기 전에 불러요.
 * 출력: 지금 명령과 기다리던 명령 모두 done에 Gone을 넘겨요.
 * 실패: 없어요.
 */
    void stop();
private:
    struct Private;
    std::unique_ptr<Private> d_;
};
}
