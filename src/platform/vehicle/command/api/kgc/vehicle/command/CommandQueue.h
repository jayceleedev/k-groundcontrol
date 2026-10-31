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
/** 명령 하나의 ACK 대기 방식을 정해요.
 * 입력: 한 번 보낸 뒤 기다릴 timeout과 재시도 횟수 retries예요.
 * 출력: 기본값 1200ms, 3회예요. retries가 3이면 많아야 네 번 나가요.
 * 실패: 없어요.
 */
struct CommandOptions {
    std::chrono::milliseconds timeout = kgc::app::constants::kAckTimeout;
    int retries = kgc::app::constants::kAckRetries;
};
/** ACK 없이 끝난 까닭을 나타내요.
 * 입력: 재시도까지 응답이 없거나, 큐가 멈췄거나, 보낼 링크가 없는 상황이에요.
 * 출력: Timeout·Gone·NoLink 중 하나예요.
 * 실패: 없어요.
 */
enum class CommandFailure { Timeout, Gone, NoLink };
/** 명령 하나의 끝 결과예요.
 * 입력: 큐가 채워요.
 * 출력: ACK를 받았으면 result에 값이 있고, 아니면 failure에 까닭이 있어요.
 * 실패: 없어요.
 */
struct CommandResult {
    std::optional<MAV_RESULT> result;
    std::optional<CommandFailure> failure;
};
/** 기체 하나의 명령을 차례로 보내고 ACK를 기다려요.
 * 입력: 송신 함수 send, 대상 sysid, Vehicle이 넘기는 COMMAND_ACK예요.
 * 출력: 명령마다 done을 정확히 한 번 불러요.
 * 실패: 응답 없음·큐 멈춤·링크 없음은 CommandResult의 failure로 알려요.
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
/** 명령을 줄 세워요.
 * 입력: 명령 command, options, 끝날 때 부를 done, done을 부를 문맥 context예요.
 * 출력: 앞 명령이 끝나면 OutboundKind::Command로 보내요. 시간 재기는 실제 송신부터예요.
 * 실패: 큐가 멈춘 뒤면 done에 Gone을 넘겨요. context가 지워졌으면 done을 부르지 않아요.
 */
    void send(CommandLong command, CommandOptions options, std::function<void(CommandResult)> done, QObject* context);
/** 기체에서 온 COMMAND_ACK를 받아요.
 * 입력: ACK의 명령 번호 command와 결과 result예요. Vehicle::consume이 넘겨요.
 * 출력: 지금 명령과 번호가 같으면 끝내고 다음 명령을 보내요. MAV_RESULT_IN_PROGRESS는 시간을 다시 재고 끝으로 보지 않아요.
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
