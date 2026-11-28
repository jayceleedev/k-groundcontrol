// [0.6.0 §12 1] specs/features/0.6.0-fly.md
// [0.6.0 5.5 T6 T7] specs/features/0.6.0-fly.md
#include "ArduPilotPlugin.h"
#include "Constants.h"
#include "Steps.h"
#include <kgc/app/constants/safety.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <cmath>
#include <limits>
namespace kgc::vehicle::firmware {
namespace {
using kgc::app::geo::AltitudeFrame;
constexpr float kNoValue = std::numeric_limits<float>::quiet_NaN();
// 모드 번호는 ArduPilot mode.h 그대로예요. Copter 4 GUIDED·6 RTL·9 LAND·17 BRAKE,
// Plane 11 RTL·12 LOITER·13 TAKEOFF·15 GUIDED·19 QLOITER·20 QLAND·21 QRTL.
CommandStep takeoffStep(double altitude)
{
    CommandStep step;
    step.command = MAV_CMD_NAV_TAKEOFF;
    step.param7 = static_cast<float>(altitude);
    return step;
}
bool isHoverOff(std::optional<double> hoverParam)
{
    return hoverParam.has_value() && *hoverParam == 1.0;
}
}
QList<MessageInterval> ArduPilotPlugin::messageIntervals() const
{
    // ArduPilot은 EXTENDED_SYS_STATE를 기본 스트림에 넣지 않아서 1 Hz로 따로 청해요.
    return {{MAVLINK_MSG_ID_EXTENDED_SYS_STATE, kExtendedSysStateIntervalUs}};
}
bool ArduPilotPlugin::vtolStateDecidesClass(int mavType) const
{
    // 고정익은 일반 Plane과 쿼드플레인을 가르고, VTOL 19~21은 모양을 정하려고 비행 상태가 필요해요.
    // 쿼드·동축·헬리는 ArduCopter로 봐요(Q_MAV_TYPE 2·3·4 쿼드플레인도 멀티콥터로 보여요, QGC와 같음).
    switch (mavType) {
    case MAV_TYPE_FIXED_WING:
    case MAV_TYPE_VTOL_TAILSITTER_DUOROTOR: case MAV_TYPE_VTOL_TAILSITTER_QUADROTOR: case MAV_TYPE_VTOL_TILTROTOR:
        return true;
    default:
        return false;
    }
}
QList<std::uint32_t> ArduPilotPlugin::launchOnArmModes(VehicleClass vehicleClass) const
{
    // TAKEOFF 모드는 기본 TKOFF_THR_MINACC·TKOFF_THR_MINSPD 0이면 시동 즉시 추력이 올라요(Plane 4.6.3 takeoff.cpp).
    if (vehicleClass == VehicleClass::FixedWing || vehicleClass == VehicleClass::Vtol) return {13};
    return {};
}
QList<CommandStep> ArduPilotPlugin::setModeSteps(VehicleClass vehicleClass, std::uint32_t customMode) const
{
    // 쿼드플레인 RTL은 들어가자마자 QRTL·QLAND로, QRTL은 이륙 대기 중이면 QLAND로 바뀔 수 있어요(mode_rtl.cpp, mode_qrtl.cpp).
    if (vehicleClass == VehicleClass::Vtol && customMode == 11)
        return {modeStep(11, 0, {11, 20, 21})};
    if (vehicleClass == VehicleClass::Vtol && customMode == 21)
        return {modeStep(21, 0, {21, 20})};
    return {modeStep(static_cast<float>(customMode), 0, {customMode})};
}
QList<CommandStep> ArduPilotPlugin::armSteps() const { return {armStep(true, 0)}; }
QList<CommandStep> ArduPilotPlugin::disarmSteps() const { return {armStep(false, 0)}; }
QList<CommandStep> ArduPilotPlugin::emergencyStopSteps() const
{
    return {armStep(false, kgc::app::constants::kEmergencyStopMagic)};
}
TakeoffKind ArduPilotPlugin::takeoffKind(VehicleClass vehicleClass) const
{
    switch (vehicleClass) {
    case VehicleClass::Multicopter:
    case VehicleClass::Vtol: return TakeoffKind::WithAltitude;
    case VehicleClass::FixedWing: return TakeoffKind::FixedAltitude;
    default: return TakeoffKind::None;
    }
}
QString ArduPilotPlugin::takeoffAltitudeParam() const { return QStringLiteral("TKOFF_ALT"); }
AltitudeFrame ArduPilotPlugin::takeoffAltitudeFrame(VehicleClass vehicleClass) const
{
    // ArduCopter는 홈 위(mode_guided.cpp), 쿼드플레인은 지금 자리 위(QuadPlane::do_user_takeoff)로 읽어요.
    return vehicleClass == VehicleClass::Vtol ? AltitudeFrame::AboveCurrent : AltitudeFrame::RelativeHome;
}
QList<CommandStep> ArduPilotPlugin::takeoffSteps(VehicleClass vehicleClass, double altitude, bool armed) const
{
    QList<CommandStep> steps;
    switch (vehicleClass) {
    case VehicleClass::Multicopter: steps.append(modeStep(4, 0, {4})); break;
    case VehicleClass::Vtol: steps.append(modeStep(15, 0, {15})); break;
    case VehicleClass::FixedWing: steps.append(modeStep(13, 0, {13})); break;
    default: return {};
    }
    if (!armed) steps.append(armStep(true, 0));
    // 고정익은 TAKEOFF 모드가 기체 파라미터 TKOFF_ALT까지 올라가므로 NAV_TAKEOFF를 보내지 않아요.
    if (vehicleClass != VehicleClass::FixedWing) steps.append(takeoffStep(altitude));
    return steps;
}
QList<CommandStep> ArduPilotPlugin::landSteps(VehicleClass vehicleClass) const
{
    if (vehicleClass == VehicleClass::Multicopter) return {modeStep(9, 0, {9})};
    if (vehicleClass == VehicleClass::Vtol) return {modeStep(20, 0, {20})};
    return {};
}
QList<CommandStep> ArduPilotPlugin::rtlSteps(VehicleClass vehicleClass) const
{
    if (vehicleClass == VehicleClass::Multicopter) return {modeStep(6, 0, {6})};
    // Q_RTL_MODE 3이나 홈 근처 저고도면 RTL 진입 즉시 QRTL, 이륙 대기 중이면 QLAND로 바뀌어요(mode_rtl.cpp _enter).
    // 쿼드플레인이 아닌 고정익은 20·21을 보내지 않으므로 같은 목록으로 종류를 알기 전의 쿼드플레인도 덮어요.
    if (vehicleClass == VehicleClass::FixedWing || vehicleClass == VehicleClass::Vtol)
        return {modeStep(11, 0, {11, 20, 21})};
    return {};
}
QList<CommandStep> ArduPilotPlugin::pauseSteps(VehicleClass vehicleClass, FlightShape shape, const QString&) const
{
    switch (vehicleClass) {
    case VehicleClass::Multicopter: return {modeStep(17, 0, {17})};
    case VehicleClass::FixedWing: return {modeStep(12, 0, {12})};
    case VehicleClass::Vtol:
        if (shape == FlightShape::Hover) return {modeStep(19, 0, {19})};
        return {modeStep(12, 0, {12})};
    default: return {};
    }
}
AltitudeFrame ArduPilotPlugin::goToAltitudeFrame() const { return AltitudeFrame::RelativeHome; }
QList<CommandStep> ArduPilotPlugin::goToSteps(VehicleClass vehicleClass, double latDeg, double lonDeg, double altitude) const
{
    const auto guided = guidedMode(vehicleClass);
    if (!guided) return {};
    // 고정익 param4 0은 도는 방향을 WP_LOITER_RAD 부호에 맡겨요.
    const float yaw = vehicleClass == VehicleClass::Multicopter ? kNoValue : 0.0F;
    return {repositionStep(MAV_FRAME_GLOBAL_RELATIVE_ALT_INT, yaw,
        static_cast<std::int32_t>(std::lround(latDeg * kCoordinateScale)),
        static_cast<std::int32_t>(std::lround(lonDeg * kCoordinateScale)), static_cast<float>(altitude), *guided)};
}
QString ArduPilotPlugin::loiterRadiusParam() const { return QStringLiteral("WP_LOITER_RAD"); }
QString ArduPilotPlugin::vtolHoverParam() const { return QStringLiteral("Q_GUIDED_MODE"); }
bool ArduPilotPlugin::loitersAtTarget(VehicleClass vehicleClass, FlightShape, std::optional<double> hoverParam) const
{
    if (vehicleClass == VehicleClass::FixedWing) return true;
    // Q_GUIDED_MODE 1이면 쿼드플레인은 도착해서 떠 있어요. 모르면 기본값 0으로 봐요.
    if (vehicleClass == VehicleClass::Vtol) return !isHoverOff(hoverParam);
    return false;
}
LoiterCircle ArduPilotPlugin::loiterCircle(VehicleClass vehicleClass, FlightShape shape, double,
                                           std::optional<double> radiusParam, std::optional<double> hoverParam) const
{
    if (shape != FlightShape::Forward) return {};
    if (vehicleClass != VehicleClass::FixedWing && vehicleClass != VehicleClass::Vtol) return {};
    if (vehicleClass == VehicleClass::Vtol && isHoverOff(hoverParam)) return {};
    if (!radiusParam) return {};
    // WP_LOITER_RAD 절댓값이 1 이하이면 ArduPlane은 기본 60 m로 돌고, 음수면 반시계예요.
    const double size = std::abs(*radiusParam);
    return {true, size <= kLoiterRadiusSmallM ? kLoiterRadiusDefaultM : size, *radiusParam >= 0};
}
}
