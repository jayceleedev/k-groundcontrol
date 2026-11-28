// [0.6.0 5.3 T1] specs/features/0.6.0-fly.md
// [0.2.0 T6] specs/features/0.2.0-fcflash.md
#include <kgc/vehicle/command/CommandQueue.h>
#include <kgc/app/scheduler/Timeout.h>
#include <QPointer>
#include <deque>
#include <variant>
namespace kgc::vehicle::command {
namespace {
using AnyCommand = std::variant<CommandLong, CommandInt>;
std::uint16_t commandNumber(const AnyCommand& command)
{
    return std::visit([](const auto& value) { return value.command; }, command);
}
// 명령 하나를 MAVLink 메시지로 싸는 함수를 만들어요. COMMAND_INT에는 confirmation 칸이 없어 재시도는 같은 내용이에요.
kgc::vehicle::outbound::VehicleSender::PackFn packer(const AnyCommand& any, std::uint8_t confirmation, std::uint8_t target)
{
    if (const auto* command = std::get_if<CommandInt>(&any)) {
        return [command = *command, target](std::uint8_t system, std::uint8_t component,
                                            std::uint8_t channel, mavlink_message_t& message) {
            mavlink_msg_command_int_pack_chan(system, component, channel, &message,
                target, command.targetCompid, command.frame, command.command, 0, 0,
                command.param1, command.param2, command.param3, command.param4,
                command.x, command.y, command.z);
        };
    }
    return [command = std::get<CommandLong>(any), confirmation, target](std::uint8_t system, std::uint8_t component,
                                                                       std::uint8_t channel, mavlink_message_t& message) {
        mavlink_msg_command_long_pack_chan(system, component, channel, &message,
            target, command.targetCompid, command.command, confirmation,
            command.param1, command.param2, command.param3, command.param4,
            command.param5, command.param6, command.param7);
    };
}
}
struct CommandQueue::Private {
    struct Entry {
        AnyCommand command;
        CommandOptions options;
        std::function<void(CommandResult)> done;
        QPointer<QObject> context;
        int confirmation = 0;
    };
    SendFn send;
    int sysid = 0;
    CommandQueue* owner;
    kgc::app::scheduler::Timeout* timer;
    std::deque<std::shared_ptr<Entry>> pending;
    std::shared_ptr<Entry> active;
    bool stopped = false;
    bool scheduled = false;

    void deliver(const std::shared_ptr<Entry>& entry, CommandResult result)
    {
        if (!entry->context || !entry->done) return;
        QMetaObject::invokeMethod(entry->context, [entry, result] { entry->done(result); }, Qt::QueuedConnection);
    }
    void schedule()
    {
        if (scheduled || stopped || active || pending.empty()) return;
        scheduled = true;
        QMetaObject::invokeMethod(owner, [this] {
            scheduled = false;
            if (stopped || active || pending.empty()) return;
            active = std::move(pending.front());
            pending.pop_front();
            transmit();
        }, Qt::QueuedConnection);
    }
    void complete(CommandResult result)
    {
        timer->stop();
        auto entry = std::move(active);
        if (entry) deliver(entry, result);
        schedule();
    }
    void transmit()
    {
        const auto entry = active;
        const auto result = send(entry->options.kind,
            packer(entry->command, static_cast<std::uint8_t>(entry->confirmation), static_cast<std::uint8_t>(sysid)));
        // 송신 함수가 즉시 응답을 전달해도 끝난 명령의 타이머를 되살리지 않아요.
        if (active != entry) return;
        if (!result) complete({{}, CommandFailure::NoLink});
        else timer->start(entry->options.timeout);
    }
    void expired()
    {
        if (!active) return;
        if (active->confirmation >= active->options.retries) complete({{}, CommandFailure::Timeout});
        else {
            ++active->confirmation;
            transmit();
        }
    }
    // 끼어드는 명령을 앞 명령을 기다리지 않고 곧바로 보내요.
    // 끼어든 명령의 ACK와, 번호가 같은 앞 명령(같은 400)의 늦은 ACK는 구별하지 못해요. 끼어드는 것은 fly의 시동 끄기·비상 정지뿐이고
    // fly는 그 단계를 ACK 결과와 상관없이 HEARTBEAT 시동 상태로 끝내므로 받아들여요.
    // 보내고 기다리던 다른 context의 명령(연결 직후 AUTOPILOT_VERSION·HOME_POSITION 요청 같은 것)이 Preempted로 끝나면
    // 다시 넣는 것은 그 명령을 낸 쪽의 일이에요(Vehicle의 요청은 한 번 더 넣어요).
    void preempt(std::shared_ptr<Entry> entry)
    {
        timer->stop();
        if (auto previous = std::move(active)) deliver(previous, {{}, CommandFailure::Preempted});
        std::deque<std::shared_ptr<Entry>> kept;
        for (auto& waiting : pending) {
            if (waiting->context == entry->context) deliver(waiting, {{}, CommandFailure::Preempted});
            else kept.push_back(std::move(waiting));
        }
        pending = std::move(kept);
        // 이미 예약된 schedule 호출은 active가 있으면 그냥 돌아가요.
        active = std::move(entry);
        transmit();
    }
    void enqueue(AnyCommand command, CommandOptions options, std::function<void(CommandResult)> done, QObject* context)
    {
        auto entry = std::make_shared<Entry>(Entry{std::move(command), options, std::move(done), context});
        if (stopped) { deliver(entry, {{}, CommandFailure::Gone}); return; }
        if (options.preempt) { preempt(std::move(entry)); return; }
        pending.push_back(std::move(entry));
        schedule();
    }
};
CommandQueue::CommandQueue(SendFn send, int sysid, QObject* parent) : QObject(parent), d_(std::make_unique<Private>())
{
    d_->send = std::move(send);
    d_->sysid = sysid;
    d_->owner = this;
    d_->timer = new kgc::app::scheduler::Timeout(QStringLiteral("commandAck"), this);
    connect(d_->timer, &kgc::app::scheduler::Timeout::expired, this, [this] { d_->expired(); });
}
CommandQueue::~CommandQueue() { stop(); }
void CommandQueue::send(CommandLong command, CommandOptions options, std::function<void(CommandResult)> done, QObject* context)
{
    d_->enqueue(command, options, std::move(done), context);
}
void CommandQueue::send(CommandInt command, CommandOptions options, std::function<void(CommandResult)> done, QObject* context)
{
    d_->enqueue(command, options, std::move(done), context);
}
void CommandQueue::receiveAck(std::uint16_t command, MAV_RESULT result)
{
    if (!d_->active || commandNumber(d_->active->command) != command) return;
    if (result == MAV_RESULT_IN_PROGRESS) d_->timer->start(d_->active->options.timeout);
    else d_->complete({result, {}});
}
void CommandQueue::stop()
{
    if (d_->stopped) return;
    d_->stopped = true;
    d_->timer->stop();
    if (d_->active) d_->complete({{}, CommandFailure::Gone});
    auto pending = std::move(d_->pending);
    d_->pending.clear();
    for (const auto& entry : pending) d_->deliver(entry, {{}, CommandFailure::Gone});
}
}
