// [0.6.0 5.5 T5] specs/features/0.6.0-fly.md
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <kgc/comm/mavlink/Mavlink.h>
namespace kgc::vehicle::firmware {
namespace {
bool isVtolType(int mavType)
{
    switch (mavType) {
    case MAV_TYPE_VTOL_TAILSITTER_DUOROTOR: case MAV_TYPE_VTOL_TAILSITTER_QUADROTOR: case MAV_TYPE_VTOL_TILTROTOR:
    case MAV_TYPE_VTOL_FIXEDROTOR: case MAV_TYPE_VTOL_TAILSITTER: case MAV_TYPE_VTOL_TILTWING: case MAV_TYPE_VTOL_RESERVED5:
        return true;
    default:
        return false;
    }
}
bool isMulticopterType(int mavType)
{
    switch (mavType) {
    case MAV_TYPE_QUADROTOR: case MAV_TYPE_COAXIAL: case MAV_TYPE_HELICOPTER: case MAV_TYPE_HEXAROTOR:
    case MAV_TYPE_OCTOROTOR: case MAV_TYPE_TRICOPTER: case MAV_TYPE_DODECAROTOR: case MAV_TYPE_DECAROTOR:
        return true;
    default:
        return false;
    }
}
}
VehicleClass vehicleClassOf(int mavType, int vtolState)
{
    // 쿼드플레인은 Q_MAV_TYPE에 따라 HEARTBEAT type이 고정익·멀티콥터·VTOL 종류 어느 것이든 될 수 있어서 vtol_state를 먼저 봐요.
    // ArduCopter·PX4 멀티콥터는 vtol_state 0을 보내요.
    const bool flyingType = mavType == MAV_TYPE_FIXED_WING || isMulticopterType(mavType) || isVtolType(mavType);
    if (vtolState >= MAV_VTOL_STATE_TRANSITION_TO_FW && vtolState <= MAV_VTOL_STATE_FW && flyingType) return VehicleClass::Vtol;
    if (mavType == MAV_TYPE_FIXED_WING) return VehicleClass::FixedWing;
    if (isVtolType(mavType)) return VehicleClass::Vtol;
    if (isMulticopterType(mavType)) return VehicleClass::Multicopter;
    if (mavType <= MAV_TYPE_GENERIC) return VehicleClass::Unknown;
    return VehicleClass::Other;
}
FlightShape flightShapeOf(VehicleClass vehicleClass, int vtolState)
{
    if (vehicleClass == VehicleClass::FixedWing) return FlightShape::Forward;
    if (vehicleClass == VehicleClass::Vtol)
        return vtolState == MAV_VTOL_STATE_FW || vtolState == MAV_VTOL_STATE_TRANSITION_TO_FW ? FlightShape::Forward : FlightShape::Hover;
    return FlightShape::Hover;
}
}
