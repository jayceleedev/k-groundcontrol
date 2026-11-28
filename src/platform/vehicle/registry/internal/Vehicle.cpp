// [0.6.0 5.6] specs/features/0.6.0-fly.md
// [0.5.0 S3] specs/features/0.5.0-map.md
// [0.3.0 5.1] specs/features/0.3.0-params.md
// [0.2.0 S3 S4] specs/features/0.2.0-fcflash.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/registry/Vehicle.h>
#include <kgc/comm/links/LinkManager.h>
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <kgc/vehicle/states/VehicleStateOwner.h>
#include <kgc/vehicle/outbound/VehicleSender.h>
#include <kgc/vehicle/command/CommandQueue.h>
namespace kgc::vehicle::registry {
namespace {
// 응답 없이 끝났거나(시간 다 됨·링크 없음) 끼어든 명령에 밀린 요청만 다시 넣어요. Gone과 ACK를 받은 결과는 그대로 끝내요.
bool worthRetry(const kgc::vehicle::command::CommandResult& result)
{
    using kgc::vehicle::command::CommandFailure;
    return result.failure == CommandFailure::Timeout || result.failure == CommandFailure::NoLink
        || result.failure == CommandFailure::Preempted;
}
}
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
    // 응답을 한 번 잃어도 버전을 알 수 있게 기본 횟수만큼 다시 보내요.
    requestMessage(MAVLINK_MSG_ID_AUTOPILOT_VERSION);
    // 홈 위치도 같은 방법으로 한 번 요청해요. 큐가 하나씩 보내므로 버전 요청이 응답을 받거나 시간을 다 쓴 뒤 나가요.
    // 그 뒤 기체가 스스로 보내는 HOME_POSITION도 같은 길로 받아요.
    requestMessage(MAVLINK_MSG_ID_HOME_POSITION);
    requestMessageIntervals();
    // 비행 상태가 5초 동안 오지 않아 비워졌으면(끊김으로 잡히지 않은 채 기체가 재부팅한 때) 주기를 다시 청해요.
    // 링크가 끊겨 비운 때는 다시 이어질 때 consume이 청해요.
    connect(d_->states, &kgc::vehicle::states::VehicleStates::flightChanged, this, [this] {
        const auto flight = d_->states->flight();
        if (flight.landedState == -1 && flight.vtolState == -1 && !d_->states->link().lost) requestMessageIntervals();
    });
}
void Vehicle::requestMessageIntervals()
{
    for (const auto& interval : d_->plugin->messageIntervals()) requestMessageInterval(interval);
}
void Vehicle::requestMessageInterval(const kgc::vehicle::firmware::MessageInterval& interval, bool retry)
{
    kgc::vehicle::command::CommandLong request;
    request.command = MAV_CMD_SET_MESSAGE_INTERVAL;
    request.param1 = interval.messageId;
    request.param2 = static_cast<float>(interval.intervalUs);
    // param3은 0이어야 ArduPilot이 받아요.
    request.param3 = 0.0F;
    d_->commands->send(request, {}, [this, interval, retry](kgc::vehicle::command::CommandResult result) {
        if (retry && worthRetry(result)) requestMessageInterval(interval, false);
    }, this);
}
void Vehicle::requestMessage(std::uint16_t messageId, bool retry)
{
    kgc::vehicle::command::CommandLong request;
    request.command = MAV_CMD_REQUEST_MESSAGE;
    request.param1 = messageId;
    // 연결 직후 시동 끄기가 끼어들어 밀리면 펌웨어 버전·홈을 모른 채 남지 않게 한 번 더 넣어요(큐 끝으로 가요).
    d_->commands->send(request, {}, [this, messageId, retry](kgc::vehicle::command::CommandResult result) {
        if (retry && worthRetry(result)) requestMessage(messageId, false);
    }, this);
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
    if (lost && !d_->states->link().lost) {
        d_->sender->sendStreamRequests();
        // 기체가 재부팅하면 주기 설정이 사라지므로 다시 이어질 때 다시 청해요.
        requestMessageIntervals();
    }
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
