// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "BatteryUpdater.h"
#include "Constants.h"
namespace kgc::vehicle::states {
BatteryBundle updateBattery(const mavlink_message_t& message)
{
    mavlink_sys_status_t status{};
    mavlink_msg_sys_status_decode(&message, &status);
    BatteryBundle result;
    if (status.voltage_battery != kUnknownUnsigned16) result.voltageV = status.voltage_battery / kMilliUnits;
    if (status.current_battery != -1) result.currentA = status.current_battery / kCentimetresPerMetre;
    result.remainingPct = status.battery_remaining;
    return result;
}
}
