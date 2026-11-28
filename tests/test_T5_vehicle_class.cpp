// [0.6.0 T5] specs/features/0.6.0-fly.md
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <QtTest>
using kgc::vehicle::firmware::FlightShape;
using kgc::vehicle::firmware::VehicleClass;
using kgc::vehicle::firmware::flightShapeOf;
using kgc::vehicle::firmware::vehicleClassOf;
class VehicleClassTest : public QObject {
    Q_OBJECT
private slots:
    /** T5: type만으로 정하는 종류와, vtol_state 1~4가 고정익·멀티콥터·VTOL 종류를 VTOL로 바꾸는 규칙이에요. */
    void test_T5_vehicle_class_of()
    {
        QCOMPARE(vehicleClassOf(MAV_TYPE_QUADROTOR, -1), VehicleClass::Multicopter);
        QCOMPARE(vehicleClassOf(MAV_TYPE_HELICOPTER, -1), VehicleClass::Multicopter);
        QCOMPARE(vehicleClassOf(MAV_TYPE_FIXED_WING, -1), VehicleClass::FixedWing);
        QCOMPARE(vehicleClassOf(MAV_TYPE_FIXED_WING, 0), VehicleClass::FixedWing);
        QCOMPARE(vehicleClassOf(MAV_TYPE_FIXED_WING, 3), VehicleClass::Vtol);
        QCOMPARE(vehicleClassOf(MAV_TYPE_FIXED_WING, 4), VehicleClass::Vtol);
        QCOMPARE(vehicleClassOf(MAV_TYPE_VTOL_TILTROTOR, -1), VehicleClass::Vtol);
        QCOMPARE(vehicleClassOf(MAV_TYPE_GROUND_ROVER, -1), VehicleClass::Other);
        QCOMPARE(vehicleClassOf(MAV_TYPE_GENERIC, -1), VehicleClass::Unknown);
        QCOMPARE(vehicleClassOf(-1, -1), VehicleClass::Unknown);
        QCOMPARE(vehicleClassOf(MAV_TYPE_QUADROTOR, 3), VehicleClass::Vtol);
        QCOMPARE(vehicleClassOf(MAV_TYPE_HELICOPTER, 4), VehicleClass::Vtol);
        QCOMPARE(vehicleClassOf(MAV_TYPE_COAXIAL, 1), VehicleClass::Vtol);
        QCOMPARE(vehicleClassOf(MAV_TYPE_QUADROTOR, 0), VehicleClass::Multicopter);
        QCOMPARE(vehicleClassOf(MAV_TYPE_VTOL_TILTROTOR, 0), VehicleClass::Vtol);
        QCOMPARE(vehicleClassOf(MAV_TYPE_GROUND_ROVER, 4), VehicleClass::Other);
        QCOMPARE(vehicleClassOf(MAV_TYPE_GENERIC, 3), VehicleClass::Unknown);
    }
    /** T5: 고정익은 앞으로, VTOL은 고정익 상태·고정익으로 전환 중일 때만 앞으로 나는 모양이에요. */
    void test_T5_flight_shape_of()
    {
        QCOMPARE(flightShapeOf(VehicleClass::Multicopter, -1), FlightShape::Hover);
        QCOMPARE(flightShapeOf(VehicleClass::FixedWing, 0), FlightShape::Forward);
        QCOMPARE(flightShapeOf(VehicleClass::Vtol, 3), FlightShape::Hover);
        QCOMPARE(flightShapeOf(VehicleClass::Vtol, 4), FlightShape::Forward);
        QCOMPARE(flightShapeOf(VehicleClass::Vtol, 1), FlightShape::Forward);
        QCOMPARE(flightShapeOf(VehicleClass::Vtol, 2), FlightShape::Hover);
        QCOMPARE(flightShapeOf(VehicleClass::Other, -1), FlightShape::Hover);
    }
};
QTEST_GUILESS_MAIN(VehicleClassTest)
#include "test_T5_vehicle_class.moc"
