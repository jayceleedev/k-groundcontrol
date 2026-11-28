// [0.6.0 §12 1] specs/features/0.6.0-fly.md
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
    // vtol_state는 고정익 type에서만 일반 Plane과 쿼드플레인을 갈라요(QGC와 같은 규칙).
    // 멀티콥터 종류는 vtol_state를 보지 않아요 — ArduCopter는 늘 3(MC)을 보내요(GCS_MAVLink_Copter.h).
    const bool vtolFlying = vtolState >= MAV_VTOL_STATE_TRANSITION_TO_FW && vtolState <= MAV_VTOL_STATE_FW;
    if (vtolFlying && mavType == MAV_TYPE_FIXED_WING) return VehicleClass::Vtol;
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
