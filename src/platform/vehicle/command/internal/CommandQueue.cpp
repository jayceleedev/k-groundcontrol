// [0.2.0 T6] specs/features/0.2.0-fcflash.md
#include <kgc/vehicle/command/CommandQueue.h>
namespace kgc::vehicle::command {
struct CommandQueue::Private {
    SendFn send;
    int sysid = 0;
};
CommandQueue::CommandQueue(SendFn send, int sysid, QObject* parent) : QObject(parent), d_(std::make_unique<Private>())
{
    d_->send = std::move(send);
    d_->sysid = sysid;
}
CommandQueue::~CommandQueue() = default;
void CommandQueue::send(CommandLong command, CommandOptions options, std::function<void(CommandResult)> done, QObject* context)
{
    // WORKER-FILL: spec §5.7 — 줄에 넣고, 앞 명령이 없으면 mavlink_msg_command_long_pack_chan으로 보낸 뒤 Timeout을 시작해요.
    Q_UNUSED(command);
    Q_UNUSED(options);
    Q_UNUSED(done);
    Q_UNUSED(context);
}
void CommandQueue::receiveAck(std::uint16_t command, MAV_RESULT result)
{
    // WORKER-FILL: spec §5.7 — 번호가 같으면 끝내고 다음을 보내요. IN_PROGRESS는 시간만 다시 재요.
    Q_UNUSED(command);
    Q_UNUSED(result);
}
void CommandQueue::stop()
{
    // WORKER-FILL: spec §5.7 — 지금 명령과 줄의 명령을 Gone으로 끝내고, 뒤 send도 Gone으로 끝나게 해요.
}
}
