// [0.6.0 5.5 T4 T6 T7] specs/features/0.6.0-fly.md
#include "Px4Plugin.h"
#include "Constants.h"
#include "Steps.h"
#include <kgc/app/constants/safety.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <QVersionNumber>
#include <cmath>
#include <limits>
namespace kgc::vehicle::firmware {
namespace {
using kgc::app::geo::AltitudeFrame;
constexpr float kNoValue = std::numeric_limits<float>::quiet_NaN();
constexpr std::int32_t kKeepCoordinate = std::numeric_limits<std::int32_t>::max();
// 모드 값은 PX4 px4_custom_mode.h의 주 모드·보조 모드예요. 1 Manual, 2 Altitude, 3 Position, 5 Acro, 7 Stabilized,
// 4 Auto의 보조 2 Takeoff·3 Hold(Loiter)·4 Mission·5 Return(RTL)·6 Land.
bool flies(VehicleClass vehicleClass)
{
    return vehicleClass == VehicleClass::Multicopter || vehicleClass == VehicleClass::FixedWing
        || vehicleClass == VehicleClass::Vtol;
}
bool hovers(VehicleClass vehicleClass)
{
    return vehicleClass == VehicleClass::Multicopter || vehicleClass == VehicleClass::Vtol;
}
}
QList<MessageInterval> Px4Plugin::messageIntervals() const
{
    // PX4는 EXTENDED_SYS_STATE 1 Hz·POSITION_TARGET_GLOBAL_INT 1 Hz·ORBIT_EXECUTION_STATUS 2 Hz를 스스로 보내요.
    return {};
}
QList<FlightMode> Px4Plugin::flightModes(VehicleClass vehicleClass) const
{
    if (!flies(vehicleClass)) return {};
    // 고정익 Position은 스틱이 가운데면 지금 방향으로 곧게 날아 공중에서 흐리게 해요(결정 5). VTOL은 종류 하나에 목록 하나라 모양과 상관없이 같아요.
    const bool positionPilot = vehicleClass != VehicleClass::Multicopter;
    QList<FlightMode> modes{
        {px4Mode(1, 0), modeName(px4Mode(1, 0), 0), true}, {px4Mode(7, 0), modeName(px4Mode(7, 0), 0), true},
        {px4Mode(5, 0), modeName(px4Mode(5, 0), 0), true}, {px4Mode(2, 0), modeName(px4Mode(2, 0), 0), true},
        {px4Mode(3, 0), modeName(px4Mode(3, 0), 0), positionPilot}, {px4Mode(4, 3), modeName(px4Mode(4, 3), 0), false},
        {px4Mode(4, 4), modeName(px4Mode(4, 4), 0), false}, {px4Mode(4, 5), modeName(px4Mode(4, 5), 0), false}};
    // 고정익 착륙은 범위 밖이라 Land를 빼요. 차례는 번호 오름차순이 아니라 PX4 화면 차례예요.
    if (vehicleClass != VehicleClass::FixedWing) modes.append({px4Mode(4, 6), modeName(px4Mode(4, 6), 0), false});
    return modes;
}
std::optional<std::uint32_t> Px4Plugin::guidedMode(VehicleClass vehicleClass) const
{
    if (flies(vehicleClass)) return px4Mode(4, 3);
    return std::nullopt;
}
bool Px4Plugin::vtolStateDecidesClass(int mavType) const
{
    // VTOL 종류 type은 모양을 vtol_state로만 알 수 있어요.
    return vehicleClassOf(mavType, -1) == VehicleClass::Vtol;
}
QList<std::uint32_t> Px4Plugin::launchOnArmModes(VehicleClass vehicleClass) const
{
    // Takeoff 모드는 시동이 걸리면 앞서 받은 높이로 올라요.
    if (hovers(vehicleClass)) return {px4Mode(4, 2)};
    return {};
}
QList<CommandStep> Px4Plugin::setModeSteps(VehicleClass, std::uint32_t customMode) const
{
    return {modeStep(static_cast<float>((customMode >> 16) & 0xff), static_cast<float>((customMode >> 24) & 0xff), {customMode})};
}
QList<CommandStep> Px4Plugin::armSteps() const { return {armStep(true, 0)}; }
QList<CommandStep> Px4Plugin::disarmSteps() const { return {armStep(false, 0)}; }
QList<CommandStep> Px4Plugin::emergencyStopSteps() const
{
    return {armStep(false, kgc::app::constants::kEmergencyStopMagic)};
}
TakeoffKind Px4Plugin::takeoffKind(VehicleClass vehicleClass) const
{
    // 고정익 이륙은 확인하지 않아 범위 밖이에요.
    return hovers(vehicleClass) ? TakeoffKind::WithAltitude : TakeoffKind::None;
}
QString Px4Plugin::takeoffAltitudeParam() const { return {}; }
AltitudeFrame Px4Plugin::takeoffAltitudeFrame(VehicleClass) const { return AltitudeFrame::Amsl; }
QList<CommandStep> Px4Plugin::takeoffSteps(VehicleClass vehicleClass, double altitude, bool armed) const
{
    if (!hovers(vehicleClass)) return {};
    // QGC 순서대로 이륙 명령을 먼저, 시동을 나중에 보내요.
    CommandStep takeoff;
    takeoff.command = MAV_CMD_NAV_TAKEOFF;
    takeoff.param1 = kNoValue;
    takeoff.param2 = kNoValue;
    takeoff.param3 = 0.0F;
    takeoff.param4 = kNoValue;
    takeoff.param5 = kNoValue;
    takeoff.param6 = kNoValue;
    takeoff.param7 = static_cast<float>(altitude);
    takeoff.confirmModes = {px4Mode(4, 2)};
    QList<CommandStep> steps{takeoff};
    if (!armed) steps.append(armStep(true, 0));
    return steps;
}
QList<CommandStep> Px4Plugin::landSteps(VehicleClass vehicleClass) const
{
    if (hovers(vehicleClass)) return {modeStep(4, 6, {px4Mode(4, 6)})};
    return {};
}
QList<CommandStep> Px4Plugin::rtlSteps(VehicleClass vehicleClass) const
{
    if (flies(vehicleClass)) return {modeStep(4, 5, {px4Mode(4, 5)})};
    return {};
}
QList<CommandStep> Px4Plugin::pauseSteps(VehicleClass vehicleClass, FlightShape, const QString& firmwareVersion) const
{
    // PX4 1.12 전은 COMMAND_INT x·y의 INT32_MAX를 214.7483647°로 읽어 그쪽으로 날아가요(QGC sendRepositionKeepLatLon).
    // 그래서 버전을 모르거나 낮으면 일시정지를 숨겨요.
    const auto version = QVersionNumber::fromString(firmwareVersion);
    if (version < QVersionNumber(kPx4KeepLatLonMajor, kPx4KeepLatLonMinor, 0) || !flies(vehicleClass)) return {};
    return {repositionStep(MAV_FRAME_GLOBAL_INT, kNoValue, kKeepCoordinate, kKeepCoordinate, kNoValue, px4Mode(4, 3))};
}
AltitudeFrame Px4Plugin::goToAltitudeFrame() const { return AltitudeFrame::Amsl; }
QList<CommandStep> Px4Plugin::goToSteps(VehicleClass vehicleClass, double latDeg, double lonDeg, double altitude) const
{
    const auto guided = guidedMode(vehicleClass);
    if (!guided) return {};
    // PX4는 프레임을 보지 않고 z를 해수면 고도로 읽어요.
    return {repositionStep(MAV_FRAME_GLOBAL_INT, kNoValue,
        static_cast<std::int32_t>(std::lround(latDeg * kCoordinateScale)),
        static_cast<std::int32_t>(std::lround(lonDeg * kCoordinateScale)), static_cast<float>(altitude), *guided)};
}
QString Px4Plugin::loiterRadiusParam() const { return QStringLiteral("NAV_LOITER_RAD"); }
QString Px4Plugin::vtolHoverParam() const { return {}; }
bool Px4Plugin::loitersAtTarget(VehicleClass vehicleClass, FlightShape shape, std::optional<double>) const
{
    if (vehicleClass == VehicleClass::FixedWing) return true;
    if (vehicleClass == VehicleClass::Vtol) return shape == FlightShape::Forward;
    return false;
}
LoiterCircle Px4Plugin::loiterCircle(VehicleClass vehicleClass, FlightShape shape, double orbitRadiusM,
                                     std::optional<double> radiusParam, std::optional<double>) const
{
    if (shape != FlightShape::Forward) return {};
    if (vehicleClass != VehicleClass::FixedWing && vehicleClass != VehicleClass::Vtol) return {};
    // ORBIT_EXECUTION_STATUS 반지름은 양수가 시계 방향이에요. 없으면 NAV_LOITER_RAD로 그려요.
    if (std::isfinite(orbitRadiusM) && orbitRadiusM != 0) return {true, std::abs(orbitRadiusM), orbitRadiusM > 0};
    if (radiusParam) return {true, std::abs(*radiusParam), true};
    return {};
}
}
