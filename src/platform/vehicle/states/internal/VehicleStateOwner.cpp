// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/states/VehicleStateOwner.h>
#include <kgc/vehicle/states/VehicleStates.h>
#include <kgc/vehicle/states/LinkLoss.h>
#include <kgc/app/scheduler/Scheduler.h>
#include <kgc/app/constants/mavlink.h>
#include "updaters/LinkUpdater.h"
#include "updaters/ArmingUpdater.h"
#include "updaters/AttitudeUpdater.h"
#include "updaters/PositionUpdater.h"
#include "updaters/GpsUpdater.h"
#include "updaters/BatteryUpdater.h"
#include "updaters/CapabilitiesUpdater.h"
namespace kgc::vehicle::states {
VehicleStateOwner::VehicleStateOwner(VehicleStates& states, const kgc::vehicle::firmware::FirmwarePlugin& plugin,
                                   kgc::app::scheduler::Scheduler& scheduler, int sysid, QObject* parent)
    : QObject(parent), states_(states), plugin_(plugin)
{
    clock_.start();
    scheduler.registerTask(QString("linkLoss.%1").arg(sysid), kgc::app::constants::kGcsHeartbeatPeriod,
                           this, this, [this] { checkLinkLoss(); });
}
void VehicleStateOwner::checkLinkLoss()
{
    const auto lost = isLinkLost(states_.link_.lastHeartbeatMs, clock_.elapsed());
    if (states_.link_.lost == lost) return;
    states_.link_.lost = lost;
    emit states_.linkChanged();
}
void VehicleStateOwner::consume(const mavlink_message_t& message)
{
    switch (message.msgid) {
    case MAVLINK_MSG_ID_HEARTBEAT:
        states_.link_ = updateLink(message, clock_.elapsed());
        states_.arming_ = updateArming(message, plugin_);
        states_.capabilities_ = updateCapabilities(message, plugin_);
        emit states_.linkChanged();
        emit states_.armingChanged();
        emit states_.capabilitiesChanged();
        break;
    case MAVLINK_MSG_ID_ATTITUDE:
        states_.attitude_ = updateAttitude(message);
        emit states_.attitudeChanged();
        break;
    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
        states_.position_ = updatePosition(message);
        emit states_.positionChanged();
        break;
    case MAVLINK_MSG_ID_GPS_RAW_INT:
        states_.gps_ = updateGps(message);
        emit states_.gpsChanged();
        break;
    case MAVLINK_MSG_ID_SYS_STATUS:
        states_.battery_ = updateBattery(message);
        emit states_.batteryChanged();
        break;
    default: break;
    }
}
}
