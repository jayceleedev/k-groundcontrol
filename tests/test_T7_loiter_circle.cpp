// [0.6.0 T7] specs/features/0.6.0-fly.md
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <QtTest>
#include <limits>
using kgc::vehicle::firmware::FirmwarePluginFactory;
using kgc::vehicle::firmware::FlightShape;
using kgc::vehicle::firmware::LoiterCircle;
using kgc::vehicle::firmware::VehicleClass;
namespace {
constexpr double kNan = std::numeric_limits<double>::quiet_NaN();
constexpr std::optional<double> kNone = std::nullopt;
const LoiterCircle kHidden{};
}
class LoiterCircleTest : public QObject {
    Q_OBJECT
private slots:
    /** T7: ArduPilot은 WP_LOITER_RAD(1 이하면 60 m, 음수면 반시계)로 그리고, 고정익 비행 모양이 아니거나 도착해서 떠 있으면 그리지 않아요. */
    void test_T7_ardupilot_circle()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        const auto forward = FlightShape::Forward;
        QCOMPARE(plugin->loiterCircle(VehicleClass::FixedWing, forward, kNan, 80.0, kNone), LoiterCircle({true, 80, true}));
        QCOMPARE(plugin->loiterCircle(VehicleClass::FixedWing, forward, kNan, -90.0, kNone), LoiterCircle({true, 90, false}));
        QCOMPARE(plugin->loiterCircle(VehicleClass::FixedWing, forward, kNan, 1.0, kNone), LoiterCircle({true, 60, true}));
        QCOMPARE(plugin->loiterCircle(VehicleClass::FixedWing, forward, kNan, -1.0, kNone), LoiterCircle({true, 60, false}));
        QCOMPARE(plugin->loiterCircle(VehicleClass::FixedWing, forward, kNan, kNone, kNone).show, false);
        QCOMPARE(plugin->loiterCircle(VehicleClass::Multicopter, FlightShape::Hover, kNan, 80.0, kNone).show, false);
        QCOMPARE(plugin->loiterCircle(VehicleClass::Vtol, forward, kNan, 80.0, 0.0), LoiterCircle({true, 80, true}));
        QCOMPARE(plugin->loiterCircle(VehicleClass::Vtol, forward, kNan, 80.0, 1.0), kHidden);
        QCOMPARE(plugin->loiterCircle(VehicleClass::Vtol, forward, kNan, 80.0, kNone), LoiterCircle({true, 80, true}));
        QCOMPARE(plugin->loiterCircle(VehicleClass::Vtol, FlightShape::Hover, kNan, 80.0, 0.0), kHidden);
    }
    /** T7: PX4는 ORBIT_EXECUTION_STATUS 반지름(양수 시계)을, 없으면 NAV_LOITER_RAD를 시계 방향으로 그려요. */
    void test_T7_px4_circle()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        const auto forward = FlightShape::Forward;
        QCOMPARE(plugin->loiterCircle(VehicleClass::FixedWing, forward, -120, 80.0, kNone), LoiterCircle({true, 120, false}));
        QCOMPARE(plugin->loiterCircle(VehicleClass::FixedWing, forward, 150, 80.0, kNone), LoiterCircle({true, 150, true}));
        QCOMPARE(plugin->loiterCircle(VehicleClass::FixedWing, forward, kNan, 80.0, kNone), LoiterCircle({true, 80, true}));
        QCOMPARE(plugin->loiterCircle(VehicleClass::FixedWing, forward, kNan, kNone, kNone).show, false);
        QCOMPARE(plugin->loiterCircle(VehicleClass::Vtol, FlightShape::Hover, 100, 80.0, kNone).show, false);
        QCOMPARE(plugin->loiterCircle(VehicleClass::Vtol, forward, kNan, 80.0, kNone), LoiterCircle({true, 80, true}));
    }
    /** T7: 도착해서 도는지(창의 안내 글)를 펌웨어마다 정해요. */
    void test_T7_loiters_at_target()
    {
        const auto ardupilot = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        QVERIFY(!ardupilot->loitersAtTarget(VehicleClass::Multicopter, FlightShape::Hover, kNone));
        QVERIFY(ardupilot->loitersAtTarget(VehicleClass::FixedWing, FlightShape::Forward, kNone));
        QVERIFY(ardupilot->loitersAtTarget(VehicleClass::Vtol, FlightShape::Hover, 0.0));
        QVERIFY(!ardupilot->loitersAtTarget(VehicleClass::Vtol, FlightShape::Hover, 1.0));
        QVERIFY(ardupilot->loitersAtTarget(VehicleClass::Vtol, FlightShape::Forward, kNone));
        const auto px4 = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        QVERIFY(!px4->loitersAtTarget(VehicleClass::Vtol, FlightShape::Hover, kNone));
        QVERIFY(px4->loitersAtTarget(VehicleClass::Vtol, FlightShape::Forward, kNone));
        QVERIFY(px4->loitersAtTarget(VehicleClass::FixedWing, FlightShape::Forward, kNone));
        QVERIFY(!px4->loitersAtTarget(VehicleClass::Multicopter, FlightShape::Hover, kNone));
        const auto generic = FirmwarePluginFactory::create(250);
        for (const auto vehicleClass : {VehicleClass::Multicopter, VehicleClass::FixedWing, VehicleClass::Vtol})
            for (const auto shape : {FlightShape::Hover, FlightShape::Forward})
                QVERIFY(!generic->loitersAtTarget(vehicleClass, shape, kNone));
        QCOMPARE(generic->loiterCircle(VehicleClass::FixedWing, FlightShape::Forward, 100, 80.0, kNone), kHidden);
    }
};
QTEST_GUILESS_MAIN(LoiterCircleTest)
#include "test_T7_loiter_circle.moc"
