// [0.3.0 T7] specs/features/0.3.0-params.md
#include "ParamGrouping.h"
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <QtTest>
using kgc::params::mergeSingleGroups;
using kgc::vehicle::firmware::FirmwarePluginFactory;
class ParamGroupTest : public QObject {
    Q_OBJECT
private slots:
    /** T7: PX4는 메타데이터 묶음을 쓰고, 없으면 이름의 첫 _ 앞이에요. */
    void test_T7_px4_group()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        QCOMPARE(plugin->paramGroup("BAT1_N_CELLS", "Battery Calibration"), QString("Battery Calibration"));
        QCOMPARE(plugin->paramGroup("MPC_XY_VEL", ""), QString("MPC"));
        QVERIFY(plugin->paramGroupTitle("MPC").isEmpty());
    }
    /** T7: ArduPilot은 앞머리에서 끝 숫자를 빼고, Lua Script만 메타데이터 묶음을 그대로 써요. */
    void test_T7_ardupilot_group()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        QCOMPARE(plugin->paramGroup("BATT2_CAPACITY", "BATT2_"), QString("BATT"));
        QCOMPARE(plugin->paramGroup("EK3_ENABLE", ""), QString("EK"));
        QCOMPARE(plugin->paramGroup("SCR_USER1", "Lua Script"), QString("Lua Script"));
        QVERIFY(plugin->paramGroup("FORMAT", "").isEmpty());
        QCOMPARE(plugin->paramGroupTitle("BATT"), QString::fromUtf8("배터리"));
        QVERIFY(plugin->paramGroupTitle("SIM").isEmpty());
    }
    /** T7: 범용 플러그인은 첫 _ 앞을 쓰고 제목은 없어요. */
    void test_T7_generic_group()
    {
        const auto plugin = FirmwarePluginFactory::create(250);
        QCOMPARE(plugin->paramGroup("ABC_DEF", "Other"), QString("ABC"));
        QVERIFY(plugin->paramGroup("ABC", "").isEmpty());
    }
    /** T7: 구성원이 하나인 묶음과 빈 이름은 기타(빈 문자열)가 되고 순서는 그대로예요. */
    void test_T7_merge_single()
    {
        QCOMPARE(mergeSingleGroups({"A", "B", "A", "", "C", "C"}), QStringList({"A", "", "A", "", "C", "C"}));
    }
};
QTEST_APPLESS_MAIN(ParamGroupTest)
#include "test_T7_param_group.moc"
