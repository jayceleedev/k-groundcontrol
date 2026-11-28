// [0.6.0 T4] specs/features/0.6.0-fly.md
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <QtTest>
using kgc::vehicle::firmware::FirmwarePlugin;
using kgc::vehicle::firmware::FirmwarePluginFactory;
using kgc::vehicle::firmware::FlightMode;
using kgc::vehicle::firmware::VehicleClass;
namespace {
using Numbers = QList<std::uint32_t>;
Numbers numbersOf(const QList<FlightMode>& modes, bool pilotOnly = false)
{
    Numbers numbers;
    for (const auto& mode : modes)
        if (!pilotOnly || mode.pilotControlled) numbers << mode.customMode;
    return numbers;
}
// 양방향 확인에 쓰는 종류별 대표 HEARTBEAT type이에요.
std::uint8_t typeOf(VehicleClass vehicleClass)
{
    switch (vehicleClass) {
    case VehicleClass::Multicopter: return MAV_TYPE_QUADROTOR;
    case VehicleClass::FixedWing: return MAV_TYPE_FIXED_WING;
    case VehicleClass::Vtol: return MAV_TYPE_VTOL_TILTROTOR;
    default: return MAV_TYPE_GENERIC;
    }
}
const Numbers kPx4Multicopter{65536, 458752, 327680, 131072, 196608, 50593792, 67371008, 84148224, 100925440};
}
class FlightModesTest : public QObject {
    Q_OBJECT
private slots:
    /** T4: ArduPilot 모드 이름은 기체 종류에 따라 Copter 표나 Plane 표로 읽어요. */
    void test_T4_ardupilot_mode_names()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        QCOMPARE(plugin->modeName(15, MAV_TYPE_FIXED_WING), QString("GUIDED"));
        QCOMPARE(plugin->modeName(19, MAV_TYPE_FIXED_WING), QString("QLOITER"));
        QCOMPARE(plugin->modeName(5, MAV_TYPE_FIXED_WING), QString("FBWA"));
        QCOMPARE(plugin->modeName(26, MAV_TYPE_FIXED_WING), QString("AUTOLAND"));
        QCOMPARE(plugin->modeName(9, MAV_TYPE_FIXED_WING), QString(""));
        QCOMPARE(plugin->modeName(15, MAV_TYPE_VTOL_TILTROTOR), QString("GUIDED"));
        QCOMPARE(plugin->modeName(4, MAV_TYPE_QUADROTOR), QString("GUIDED"));
        QCOMPARE(plugin->modeName(4, MAV_TYPE_GROUND_ROVER), QString(""));
    }
    /** T4: 모드 내림 목록의 번호 차례가 정확히 정한 대로예요. */
    void test_T4_mode_order()
    {
        const auto ardupilot = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        QCOMPARE(numbersOf(ardupilot->flightModes(VehicleClass::Multicopter)), Numbers({0, 1, 2, 3, 4, 5, 6, 7, 9, 11, 13, 16, 17, 21}));
        QCOMPARE(numbersOf(ardupilot->flightModes(VehicleClass::FixedWing)), Numbers({0, 1, 2, 4, 5, 6, 7, 10, 11, 12, 13, 15}));
        QCOMPARE(numbersOf(ardupilot->flightModes(VehicleClass::Vtol)),
            Numbers({0, 1, 2, 4, 5, 6, 7, 10, 11, 12, 13, 15, 17, 18, 19, 20, 21}));
        QCOMPARE(numbersOf(ardupilot->flightModes(VehicleClass::Other)), Numbers());
        const auto px4 = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        QCOMPARE(numbersOf(px4->flightModes(VehicleClass::Multicopter)), kPx4Multicopter);
        auto fixedWing = kPx4Multicopter;
        fixedWing.removeAll(100925440);
        QCOMPARE(numbersOf(px4->flightModes(VehicleClass::FixedWing)), fixedWing);
        const auto generic = FirmwarePluginFactory::create(250);
        QCOMPARE(numbersOf(generic->flightModes(VehicleClass::Multicopter)), Numbers());
    }
    /** T4: 조종기 스틱을 따르는 모드(공중에서 흐림)가 정확히 정한 번호예요. */
    void test_T4_pilot_controlled()
    {
        const auto ardupilot = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        QCOMPARE(numbersOf(ardupilot->flightModes(VehicleClass::Multicopter), true), Numbers({0, 1, 2, 11, 13}));
        QCOMPARE(numbersOf(ardupilot->flightModes(VehicleClass::FixedWing), true), Numbers({0, 2, 4, 5, 6, 7}));
        QCOMPARE(numbersOf(ardupilot->flightModes(VehicleClass::Vtol), true), Numbers({0, 2, 4, 5, 6, 7, 17, 18}));
        const auto px4 = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        QCOMPARE(numbersOf(px4->flightModes(VehicleClass::Multicopter), true), Numbers({65536, 458752, 327680, 131072}));
        const Numbers position{65536, 458752, 327680, 131072, 196608};
        QCOMPARE(numbersOf(px4->flightModes(VehicleClass::FixedWing), true), position);
        QCOMPARE(numbersOf(px4->flightModes(VehicleClass::Vtol), true), position);
    }
    /** T4: 모든 플러그인·종류의 모든 항목 이름이 modeName(번호, 대표 type)과 같아요. */
    void test_T4_names_match_mode_table()
    {
        for (const auto autopilot : {std::uint8_t(MAV_AUTOPILOT_ARDUPILOTMEGA), std::uint8_t(MAV_AUTOPILOT_PX4), std::uint8_t(250)}) {
            const auto plugin = FirmwarePluginFactory::create(autopilot);
            for (const auto vehicleClass : {VehicleClass::Unknown, VehicleClass::Multicopter, VehicleClass::FixedWing,
                                            VehicleClass::Vtol, VehicleClass::Other}) {
                for (const auto& mode : plugin->flightModes(vehicleClass)) {
                    QVERIFY(!mode.name.isEmpty());
                    QCOMPARE(plugin->modeName(mode.customMode, typeOf(vehicleClass)), mode.name);
                }
            }
        }
    }
    /** T4: 여기로 가기 목표가 살아 있는 유도 모드예요. */
    void test_T4_guided_mode()
    {
        const auto ardupilot = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        QCOMPARE(ardupilot->guidedMode(VehicleClass::Multicopter), std::optional<std::uint32_t>(4));
        QCOMPARE(ardupilot->guidedMode(VehicleClass::FixedWing), std::optional<std::uint32_t>(15));
        QCOMPARE(ardupilot->guidedMode(VehicleClass::Vtol), std::optional<std::uint32_t>(15));
        QVERIFY(!ardupilot->guidedMode(VehicleClass::Other).has_value());
        const auto px4 = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        for (const auto vehicleClass : {VehicleClass::Multicopter, VehicleClass::FixedWing, VehicleClass::Vtol})
            QCOMPARE(px4->guidedMode(vehicleClass), std::optional<std::uint32_t>(50593792));
        const auto generic = FirmwarePluginFactory::create(250);
        QVERIFY(!generic->guidedMode(VehicleClass::Multicopter).has_value());
    }
};
QTEST_GUILESS_MAIN(FlightModesTest)
#include "test_T4_flight_modes.moc"
