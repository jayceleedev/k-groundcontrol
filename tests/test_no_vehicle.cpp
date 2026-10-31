// [0.1.1 T6] specs/features/0.1.1-link-hardening.md
#include <kgc/link/LinkService.h>
#include <QtTest>
using kgc::link::noVehicleTooLong;
class NoVehicleTest : public QObject {
    Q_OBJECT
private slots:
    /** T6: 링크가 열린 뒤 10초가 되도록 기체가 없을 때만 참이에요. */
    void test_T6_ten_seconds_without_vehicle()
    {
        QVERIFY(!noVehicleTooLong(1000, 10999, false));
        QVERIFY(noVehicleTooLong(1000, 11000, false));
        QVERIFY(!noVehicleTooLong(1000, 20000, true));
        QVERIFY(!noVehicleTooLong(-1, 20000, false));
    }
};
QTEST_GUILESS_MAIN(NoVehicleTest)
#include "test_no_vehicle.moc"
