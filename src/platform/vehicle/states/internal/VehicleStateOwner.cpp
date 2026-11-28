// [0.6.0 5.4] specs/features/0.6.0-fly.md
// [0.5.0 S3] specs/features/0.5.0-map.md
// [0.2.0 S3 S4] specs/features/0.2.0-fcflash.md
// [0.1.1 T1 T2 T3] specs/features/0.1.1-link-hardening.md
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
#include "updaters/HomePositionUpdater.h"
#include "updaters/GpsUpdater.h"
#include "updaters/BatteryUpdater.h"
#include "updaters/CapabilitiesUpdater.h"
#include "updaters/AutopilotVersionUpdater.h"
#include "updaters/FlightUpdater.h"
#include "updaters/NavTargetUpdater.h"
#include "updaters/StatusTextUpdater.h"
#include "Constants.h"
#include <limits>
namespace kgc::vehicle::states {
VehicleStateOwner::VehicleStateOwner(VehicleStates& states, const kgc::vehicle::firmware::FirmwarePlugin& plugin,
                                   kgc::app::scheduler::Scheduler& scheduler, int sysid, QObject* parent)
    : QObject(parent), states_(states), plugin_(plugin)
{
    clock_.start();
    scheduler.registerTask(QString("linkLoss.%1").arg(sysid), kgc::app::constants::kGcsHeartbeatPeriod,
                           this, this, [this] { checkLinkLoss(); expireFlight(); expireNavTarget(); });
}
void VehicleStateOwner::checkLinkLoss()
{
    const auto lost = isLinkLost(states_.link_.lastMessageMs, clock_.elapsed());
    if (states_.link_.lost == lost) return;
    states_.link_.lost = lost;
    emit states_.linkChanged();
    // 끊긴 동안 마지막 착륙·VTOL 상태가 단추를 정하지 않게 비워요.
    if (lost) clearFlight();
}
void VehicleStateOwner::clearFlight()
{
    flightAtMs_ = -1;
    if (states_.flight_.landedState == -1 && states_.flight_.vtolState == -1) return;
    states_.flight_ = {};
    emit states_.flightChanged();
}
void VehicleStateOwner::expireFlight()
{
    if (flightAtMs_ >= 0 && isStale(flightAtMs_, clock_.elapsed(), kFlightStale)) clearFlight();
}
void VehicleStateOwner::expireNavTarget()
{
    // NaN끼리 비교하지 않고 받은 때만 봐서, 목표가 없을 때 1초마다 알림이 나지 않게 해요.
    constexpr double unknown = std::numeric_limits<double>::quiet_NaN();
    const auto now = clock_.elapsed();
    bool cleared = false;
    if (targetAtMs_ >= 0 && isStale(targetAtMs_, now, kNavTargetStale)) {
        states_.navTarget_.latDeg = unknown;
        states_.navTarget_.lonDeg = unknown;
        states_.navTarget_.altM = unknown;
        targetAtMs_ = -1;
        cleared = true;
    }
    if (orbitAtMs_ >= 0 && isStale(orbitAtMs_, now, kNavTargetStale)) {
        states_.navTarget_.orbitRadiusM = unknown;
        states_.navTarget_.orbitLatDeg = unknown;
        states_.navTarget_.orbitLonDeg = unknown;
        orbitAtMs_ = -1;
        cleared = true;
    }
    if (cleared) emit states_.navTargetChanged();
}
void VehicleStateOwner::consumeNavigation(const mavlink_message_t& message)
{
    switch (message.msgid) {
    case MAVLINK_MSG_ID_EXTENDED_SYS_STATE:
        states_.flight_ = updateFlight(message);
        flightAtMs_ = clock_.elapsed();
        emit states_.flightChanged();
        break;
    case MAVLINK_MSG_ID_POSITION_TARGET_GLOBAL_INT:
        states_.navTarget_ = updateNavTarget(message, states_.navTarget_);
        targetAtMs_ = clock_.elapsed();
        emit states_.navTargetChanged();
        break;
    case MAVLINK_MSG_ID_ORBIT_EXECUTION_STATUS:
        states_.navTarget_ = updateOrbit(message, states_.navTarget_);
        orbitAtMs_ = clock_.elapsed();
        emit states_.navTargetChanged();
        break;
    case MAVLINK_MSG_ID_STATUSTEXT:
        emit states_.vehicleMessage(statusTextOf(message));
        break;
    default: break;
    }
}
void VehicleStateOwner::consume(const mavlink_message_t& message)
{
    const auto wasLost = states_.link_.lost;
    states_.link_ = updateLink(message, clock_.elapsed());
    if (message.msgid == MAVLINK_MSG_ID_HEARTBEAT || wasLost != states_.link_.lost)
        emit states_.linkChanged();
    switch (message.msgid) {
    case MAVLINK_MSG_ID_HEARTBEAT: {
        const auto version = states_.capabilities_.firmwareVersion;
        states_.arming_ = updateArming(message, plugin_);
        states_.capabilities_ = updateCapabilities(message, plugin_);
        states_.capabilities_.firmwareVersion = version;
        emit states_.armingChanged();
        emit states_.capabilitiesChanged();
        break;
    }
    case MAVLINK_MSG_ID_AUTOPILOT_VERSION:
        states_.capabilities_ = updateAutopilotVersion(message, states_.capabilities_);
        emit states_.capabilitiesChanged();
        break;
    case MAVLINK_MSG_ID_ATTITUDE:
        states_.attitude_ = updateAttitude(message);
        emit states_.attitudeChanged();
        break;
    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
        states_.position_ = updatePosition(message, states_.position_);
        emit states_.positionChanged();
        break;
    case MAVLINK_MSG_ID_HOME_POSITION:
        states_.position_ = updateHomePosition(message, states_.position_);
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
    default:
        consumeNavigation(message);
        break;
    }
}
}
