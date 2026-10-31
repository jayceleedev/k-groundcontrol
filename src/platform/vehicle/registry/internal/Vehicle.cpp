// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/registry/Vehicle.h>
#include <kgc/comm/links/LinkManager.h>
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <kgc/vehicle/states/VehicleStateOwner.h>
#include <kgc/vehicle/outbound/VehicleSender.h>
namespace kgc::vehicle::registry {
struct Vehicle::Private {
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
}
Vehicle::~Vehicle() { stop(); }
void Vehicle::stop()
{
    // 지연 삭제 전에 반복 작업 소유자를 끝내야 같은 sysid의 새 기체를 바로 등록할 수 있어요.
    delete d_->sender;
    d_->sender = nullptr;
    delete d_->owner;
    d_->owner = nullptr;
}
void Vehicle::consume(const __mavlink_message& message)
{
    const bool lost = d_->states->link().lost;
    d_->owner->consume(message);
    if (lost && !d_->states->link().lost) d_->sender->sendStreamRequests();
}
int Vehicle::sysid() const { return d_->sysid; }
std::uint8_t Vehicle::autopilot() const { return d_->autopilot; }
kgc::vehicle::states::VehicleStates* Vehicle::states() const { return d_->states; }
kgc::comm::links::LinkConfig Vehicle::linkConfig() const { return d_->config; }
}
