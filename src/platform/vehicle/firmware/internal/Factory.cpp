// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include "px4/Px4Plugin.h"
#include "ardupilot/ArduPilotPlugin.h"
#include "generic/GenericPlugin.h"
namespace kgc::vehicle::firmware {
std::unique_ptr<FirmwarePlugin> FirmwarePluginFactory::create(std::uint8_t autopilot)
{
    if (autopilot == MAV_AUTOPILOT_PX4) return std::make_unique<Px4Plugin>();
    if (autopilot == MAV_AUTOPILOT_ARDUPILOTMEGA) return std::make_unique<ArduPilotPlugin>();
    return std::make_unique<GenericPlugin>();
}
}
