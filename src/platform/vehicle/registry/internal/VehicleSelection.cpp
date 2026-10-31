// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/registry/VehicleSelection.h>
#include <kgc/comm/mavlink/Mavlink.h>
namespace kgc::vehicle::registry {
bool VehicleSelection::accepts(std::uint8_t compid, std::uint8_t autopilot, std::uint8_t mavType)
{
    return compid == MAV_COMP_ID_AUTOPILOT1 && autopilot != MAV_AUTOPILOT_INVALID
        && mavType != MAV_TYPE_GCS && mavType != MAV_TYPE_GIMBAL;
}
bool VehicleSelection::recreate(std::uint8_t previous, std::uint8_t current) { return previous != current; }
}
