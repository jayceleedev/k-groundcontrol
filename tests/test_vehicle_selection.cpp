// [0.1.0 T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/registry/VehicleSelection.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <QtTest>
using kgc::vehicle::registry::VehicleSelection;
class VehicleSelectionTest : public QObject {
    Q_OBJECT
private slots:
    /** T6: 자동조종기 하트비트만 받아들이고 GCS와 짐벌은 걸러요. */
    void test_T6_selection()
    {
        QVERIFY(VehicleSelection::accepts(1, MAV_AUTOPILOT_PX4, MAV_TYPE_QUADROTOR));
        QVERIFY(VehicleSelection::accepts(1, 250, MAV_TYPE_QUADROTOR));
        QVERIFY(!VehicleSelection::accepts(2, MAV_AUTOPILOT_PX4, MAV_TYPE_QUADROTOR));
        QVERIFY(!VehicleSelection::accepts(1, MAV_AUTOPILOT_INVALID, MAV_TYPE_QUADROTOR));
        QVERIFY(!VehicleSelection::accepts(1, MAV_AUTOPILOT_PX4, MAV_TYPE_GCS));
        QVERIFY(!VehicleSelection::accepts(1, MAV_AUTOPILOT_PX4, MAV_TYPE_GIMBAL));
    }
    /** T6: 같은 시스템의 펌웨어가 바뀌었을 때만 재생성을 요청해요. */
    void test_T6_recreate()
    {
        QVERIFY(!VehicleSelection::recreate(MAV_AUTOPILOT_PX4, MAV_AUTOPILOT_PX4));
        QVERIFY(VehicleSelection::recreate(MAV_AUTOPILOT_PX4, MAV_AUTOPILOT_ARDUPILOTMEGA));
        QVERIFY(VehicleSelection::recreate(250, MAV_AUTOPILOT_PX4));
    }
};
QTEST_GUILESS_MAIN(VehicleSelectionTest)
#include "test_vehicle_selection.moc"
