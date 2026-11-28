// [0.6.0 5.5 T4] specs/features/0.6.0-fly.md
// [0.1.1 T1 T2 T3] specs/features/0.1.1-link-hardening.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include "ArduPilotPlugin.h"
#include <kgc/comm/mavlink/Mavlink.h>
#include <array>
#include <utility>
namespace kgc::vehicle::firmware {
namespace {
// ArduCopter 모드 표예요(번호 = custom_mode).
QString copterName(std::uint32_t customMode)
{
    constexpr std::array names{"STABILIZE", "ACRO", "ALT_HOLD", "AUTO", "GUIDED", "LOITER", "RTL",
        "CIRCLE", "", "LAND", "", "DRIFT", "", "SPORT", "FLIP", "AUTOTUNE", "POSHOLD", "BRAKE",
        "THROW", "AVOID_ADSB", "GUIDED_NOGPS", "SMART_RTL", "FLOWHOLD", "FOLLOW", "ZIGZAG",
        "SYSTEMID", "AUTOROTATE", "AUTO_RTL", "TURTLE"};
    return customMode < names.size() ? QString::fromLatin1(names[customMode]) : QString{};
}
// ArduPlane 4.7 mode.h 열거 이름이에요. 5·6은 ArduPilot 표시 이름 FBWA·FBWB예요.
QString planeName(std::uint32_t customMode)
{
    constexpr std::array names{"MANUAL", "CIRCLE", "STABILIZE", "TRAINING", "ACRO", "FBWA", "FBWB",
        "CRUISE", "AUTOTUNE", "", "AUTO", "RTL", "LOITER", "TAKEOFF", "AVOID_ADSB", "GUIDED",
        "INITIALISING", "QSTABILIZE", "QHOVER", "QLOITER", "QLAND", "QRTL", "QAUTOTUNE", "QACRO",
        "THERMAL", "LOITER_ALT_QLAND", "AUTOLAND"};
    return customMode < names.size() ? QString::fromLatin1(names[customMode]) : QString{};
}
// (번호, 스틱을 따르는지) 목록을 이름 표와 같은 글자의 모드 목록으로 만들어요.
QList<FlightMode> modesOf(std::initializer_list<std::pair<std::uint32_t, bool>> entries, QString (*name)(std::uint32_t))
{
    QList<FlightMode> modes;
    for (const auto& [customMode, pilot] : entries) modes.append({customMode, name(customMode), pilot});
    return modes;
}
QList<FlightMode> planeModes()
{
    return modesOf({{0, true}, {1, false}, {2, true}, {4, true}, {5, true}, {6, true}, {7, true},
        {10, false}, {11, false}, {12, false}, {13, false}, {15, false}}, planeName);
}
}
QString ArduPilotPlugin::modeName(std::uint32_t customMode, std::uint8_t mavType) const
{
    if (mavType == MAV_TYPE_GENERIC) return copterName(customMode);
    switch (vehicleClassOf(mavType, -1)) {
    case VehicleClass::Multicopter: return copterName(customMode);
    case VehicleClass::FixedWing:
    case VehicleClass::Vtol: return planeName(customMode);
    default: return {};
    }
}
QList<FlightMode> ArduPilotPlugin::flightModes(VehicleClass vehicleClass) const
{
    switch (vehicleClass) {
    case VehicleClass::Multicopter:
        return modesOf({{0, true}, {1, true}, {2, true}, {3, false}, {4, false}, {5, false}, {6, false},
            {7, false}, {9, false}, {11, true}, {13, true}, {16, false}, {17, false}, {21, false}}, copterName);
    case VehicleClass::FixedWing: return planeModes();
    case VehicleClass::Vtol:
        return planeModes() + modesOf({{17, true}, {18, true}, {19, false}, {20, false}, {21, false}}, planeName);
    default: return {};
    }
}
std::optional<std::uint32_t> ArduPilotPlugin::guidedMode(VehicleClass vehicleClass) const
{
    switch (vehicleClass) {
    case VehicleClass::Multicopter: return 4;
    case VehicleClass::FixedWing:
    case VehicleClass::Vtol: return 15;
    default: return std::nullopt;
    }
}
}
