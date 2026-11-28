// [0.6.0 5.5 T6 T7] specs/features/0.6.0-fly.md
// [0.4.2 5.1] specs/features/0.4.2-param-tree.md
// [0.3.0 5.2] specs/features/0.3.0-params.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "GenericPlugin.h"
#include "Steps.h"
#include <kgc/app/constants/safety.h>
#include <kgc/comm/mavlink/Mavlink.h>
namespace kgc::vehicle::firmware {
QString GenericPlugin::displayName() const { return "MAVLink"; }
QString GenericPlugin::modeName(std::uint32_t, std::uint8_t) const { return {}; }
QList<StreamRequest> GenericPlugin::streamRequests() const { return {}; }
ParamEncoding GenericPlugin::paramEncoding() const { return ParamEncoding::CCast; }
ParamMetadataSource GenericPlugin::paramMetadataSource(int, const QString&) const
{
    return {ParamMetadataFormat::None, {}, {}};
}
QString GenericPlugin::paramGroupTitle(const QString&) const { return {}; }
QStringList GenericPlugin::paramFileSkip() const { return {}; }
// 범용 플러그인은 모드·이륙·여기로 가기를 모르므로 시동·해제·비상 정지만 내요.
QList<MessageInterval> GenericPlugin::messageIntervals() const { return {}; }
bool GenericPlugin::vtolStateDecidesClass(int mavType) const { return mavType == MAV_TYPE_FIXED_WING; }
QList<std::uint32_t> GenericPlugin::launchOnArmModes(VehicleClass) const { return {}; }
QList<FlightMode> GenericPlugin::flightModes(VehicleClass) const { return {}; }
std::optional<std::uint32_t> GenericPlugin::guidedMode(VehicleClass) const { return std::nullopt; }
QList<CommandStep> GenericPlugin::setModeSteps(VehicleClass, std::uint32_t) const { return {}; }
QList<CommandStep> GenericPlugin::armSteps() const { return {armStep(true, 0)}; }
QList<CommandStep> GenericPlugin::disarmSteps() const { return {armStep(false, 0)}; }
QList<CommandStep> GenericPlugin::emergencyStopSteps() const
{
    return {armStep(false, kgc::app::constants::kEmergencyStopMagic)};
}
TakeoffKind GenericPlugin::takeoffKind(VehicleClass) const { return TakeoffKind::None; }
QString GenericPlugin::takeoffAltitudeParam() const { return {}; }
kgc::app::geo::AltitudeFrame GenericPlugin::takeoffAltitudeFrame(VehicleClass) const
{
    return kgc::app::geo::AltitudeFrame::RelativeHome;
}
QList<CommandStep> GenericPlugin::takeoffSteps(VehicleClass, double, bool) const { return {}; }
QList<CommandStep> GenericPlugin::landSteps(VehicleClass) const { return {}; }
QList<CommandStep> GenericPlugin::rtlSteps(VehicleClass) const { return {}; }
QList<CommandStep> GenericPlugin::pauseSteps(VehicleClass, FlightShape, const QString&) const { return {}; }
kgc::app::geo::AltitudeFrame GenericPlugin::goToAltitudeFrame() const { return kgc::app::geo::AltitudeFrame::RelativeHome; }
QList<CommandStep> GenericPlugin::goToSteps(VehicleClass, double, double, double) const { return {}; }
QString GenericPlugin::loiterRadiusParam() const { return {}; }
QString GenericPlugin::vtolHoverParam() const { return {}; }
bool GenericPlugin::loitersAtTarget(VehicleClass, FlightShape, std::optional<double>) const { return false; }
LoiterCircle GenericPlugin::loiterCircle(VehicleClass, FlightShape, double, std::optional<double>, std::optional<double>) const
{
    return {};
}
}
