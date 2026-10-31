// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/registry/VehicleRegistry.h>
#include <kgc/vehicle/registry/VehicleSelection.h>
#include <kgc/comm/links/LinkManager.h>
namespace kgc::vehicle::registry {
VehicleRegistry::VehicleRegistry(kgc::comm::links::LinkManager& links, kgc::app::scheduler::Scheduler& scheduler,
                                 QObject* parent)
    : QObject(parent), links_(links), scheduler_(scheduler)
{
    connect(&links, &kgc::comm::links::LinkManager::messageReceived, this,
            [this](int linkId, const mavlink_message_t& message) { receive(linkId, message); });
    connect(&links, &kgc::comm::links::LinkManager::linkClosed, this, [this] {
        const auto ids = vehicles_.keys();
        for (const auto id : ids) remove(id);
    });
}
void VehicleRegistry::receive(int linkId, const __mavlink_message& message)
{
    if (linkId != links_.linkId() || message.compid != MAV_COMP_ID_AUTOPILOT1) return;
    auto* vehicle = vehicles_.value(message.sysid).data();
    bool added = false;
    if (message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
        mavlink_heartbeat_t heartbeat{};
        mavlink_msg_heartbeat_decode(&message, &heartbeat);
        if (!VehicleSelection::accepts(message.compid, heartbeat.autopilot, heartbeat.type)) return;
        if (vehicle && VehicleSelection::recreate(vehicle->autopilot(), heartbeat.autopilot)) {
            remove(message.sysid);
            vehicle = nullptr;
        }
        if (!vehicle) {
            vehicle = new Vehicle(message.sysid, heartbeat.autopilot, links_, scheduler_, this);
            vehicles_.insert(message.sysid, vehicle);
            added = true;
        }
    }
    if (!vehicle) return;
    vehicle->consume(message);
    if (added) emit vehicleAdded(vehicle);
    if (!active_) {
        active_ = vehicle;
        emit activeChanged();
    }
}
void VehicleRegistry::remove(int sysid)
{
    auto vehicle = vehicles_.take(sysid);
    if (!vehicle) return;
    vehicle->stop();
    emit vehicleRemoved(vehicle);
    if (active_ == vehicle) {
        active_ = vehicles_.isEmpty() ? nullptr : vehicles_.first().data();
        emit activeChanged();
    }
    vehicle->deleteLater();
}
Vehicle* VehicleRegistry::active() const { return active_; }
QList<QPointer<Vehicle>> VehicleRegistry::vehicles() const { return vehicles_.values(); }
}
