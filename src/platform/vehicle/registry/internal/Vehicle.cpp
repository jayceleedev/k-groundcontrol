// [0.3.0 5.1] specs/features/0.3.0-params.md
// [0.2.0 S3 S4] specs/features/0.2.0-fcflash.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/registry/Vehicle.h>
#include <kgc/comm/links/LinkManager.h>
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <kgc/vehicle/states/VehicleStateOwner.h>
#include <kgc/vehicle/outbound/VehicleSender.h>
#include <kgc/vehicle/command/CommandQueue.h>
namespace kgc::vehicle::registry {
struct Vehicle::Private {
    kgc::vehicle::command::CommandQueue* commands = nullptr;
    int sysid;
    std::uint8_t autopilot;
    kgc::comm::links::LinkConfig config;
    std::unique_ptr<kgc::vehicle::firmware::FirmwarePlugin> plugin;
    kgc::vehicle::states::VehicleStates* states = nullptr;
    kgc::vehicle::states::VehicleStateOwner* owner = nullptr;
    kgc::vehicle::outbound::VehicleSender* sender = nullptr;
};
Vehicle::Vehicle(int sysid, std::uint8_t autopilot, kgc::comm::links::LinkManager& links,
                 kgc::app::scheduler::Scheduler& scheduler, QObject* parent)
    : QObject(parent), d_(std::make_unique<Private>())
{
    d_->sysid = sysid;
    d_->autopilot = autopilot;
    d_->config = links.linkConfig();
    d_->plugin = kgc::vehicle::firmware::FirmwarePluginFactory::create(autopilot);
    d_->states = new kgc::vehicle::states::VehicleStates(this);
    d_->owner = new kgc::vehicle::states::VehicleStateOwner(*d_->states, *d_->plugin, scheduler, sysid, this);
    d_->sender = new kgc::vehicle::outbound::VehicleSender(links, scheduler, sysid, links.linkId(), *d_->plugin, this);
    d_->commands = new kgc::vehicle::command::CommandQueue([this](auto kind, auto pack) {
        return d_->sender->send(kind, std::move(pack));
    }, sysid, this);
    kgc::vehicle::command::CommandLong request;
    request.command = MAV_CMD_REQUEST_MESSAGE;
    request.param1 = MAVLINK_MSG_ID_AUTOPILOT_VERSION;
    // 응답을 한 번 잃어도 버전을 알 수 있게 기본 횟수만큼 다시 보내요.
    d_->commands->send(request, {}, [](auto) {}, this);
}
Vehicle::~Vehicle() { stop(); }
void Vehicle::stop()
{
    // 지연 삭제 전에 반복 작업 소유자를 끝내야 같은 sysid의 새 기체를 바로 등록할 수 있어요.
    if (d_->commands) d_->commands->stop();
    delete d_->sender;
    d_->sender = nullptr;
    delete d_->owner;
    d_->owner = nullptr;
}
void Vehicle::consume(const __mavlink_message& message)
{
    if (message.msgid == MAVLINK_MSG_ID_COMMAND_ACK) {
        mavlink_command_ack_t ack{};
        mavlink_msg_command_ack_decode(&message, &ack);
        d_->commands->receiveAck(ack.command, static_cast<MAV_RESULT>(ack.result));
    }
    const bool lost = d_->states->link().lost;
    d_->owner->consume(message);
    if (lost && !d_->states->link().lost) d_->sender->sendStreamRequests();
    emit messageReceived(message);
}
kgc::vehicle::command::CommandQueue* Vehicle::commands() const { return d_->commands; }
kgc::vehicle::outbound::VehicleSender* Vehicle::sender() const { return d_->sender; }
const kgc::vehicle::firmware::FirmwarePlugin& Vehicle::plugin() const { return *d_->plugin; }
int Vehicle::sysid() const { return d_->sysid; }
std::uint8_t Vehicle::autopilot() const { return d_->autopilot; }
kgc::vehicle::states::VehicleStates* Vehicle::states() const { return d_->states; }
kgc::comm::links::LinkConfig Vehicle::linkConfig() const { return d_->config; }
}
