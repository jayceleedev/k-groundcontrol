// [0.2.0 T6] specs/features/0.2.0-fcflash.md
#include <kgc/vehicle/command/CommandQueue.h>
#include <kgc/app/scheduler/Timeout.h>
#include <QPointer>
#include <deque>
namespace kgc::vehicle::command {
struct CommandQueue::Private {
    struct Entry {
        CommandLong command;
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
        const auto command = entry->command;
        const auto confirmation = static_cast<std::uint8_t>(entry->confirmation);
        const auto target = static_cast<std::uint8_t>(sysid);
        const auto result = send(kgc::vehicle::outbound::OutboundKind::Command,
            [command, confirmation, target](std::uint8_t system, std::uint8_t component,
                                            std::uint8_t channel, mavlink_message_t& message) {
                mavlink_msg_command_long_pack_chan(system, component, channel, &message,
                    target, command.targetCompid, command.command, confirmation,
                    command.param1, command.param2, command.param3, command.param4,
                    command.param5, command.param6, command.param7);
            });
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
    auto entry = std::make_shared<Private::Entry>(Private::Entry{command, options, std::move(done), context});
    if (d_->stopped) { d_->deliver(entry, {{}, CommandFailure::Gone}); return; }
    d_->pending.push_back(std::move(entry));
    d_->schedule();
}
void CommandQueue::receiveAck(std::uint16_t command, MAV_RESULT result)
{
    if (!d_->active || d_->active->command.command != command) return;
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
