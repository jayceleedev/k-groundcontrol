// [0.1.0 T4 T5] specs/features/0.1.0-link.md
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <QtTest>
using kgc::vehicle::firmware::FirmwarePluginFactory;
class FirmwareTest : public QObject {
    Q_OBJECT
private slots:
    /** T4: 알려진 펌웨어와 모르는 값을 각각 맞는 플러그인으로 골라요. */
    void test_T4_factory()
    {
        QCOMPARE(FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4)->displayName(), QString("PX4"));
        QCOMPARE(FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA)->displayName(), QString("ArduPilot"));
        auto generic = FirmwarePluginFactory::create(250);
        QCOMPARE(generic->displayName(), QString("MAVLink"));
        QVERIFY(generic->modeName(1, MAV_TYPE_QUADROTOR).isEmpty());
        QVERIFY(generic->streamRequests().isEmpty());
    }
    /** T5: 주 모드와 보조 모드를 함께 써서 PX4 모드 이름을 찾아요. */
    void test_T5_px4_modes()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        QCOMPARE(plugin->modeName(3u << 16, MAV_TYPE_QUADROTOR), QString("Position"));
        QCOMPARE(plugin->modeName((4u << 16) | (3u << 24), MAV_TYPE_QUADROTOR), QString("Hold"));
        QCOMPARE(plugin->modeName(6u << 16, MAV_TYPE_QUADROTOR), QString("Offboard"));
        QVERIFY(plugin->modeName(255u << 16, MAV_TYPE_QUADROTOR).isEmpty());
        QVERIFY(plugin->modeName((4u << 16) | (255u << 24), MAV_TYPE_QUADROTOR).isEmpty());
        QVERIFY(plugin->streamRequests().isEmpty());
    }
    /** T5: Copter의 기체 종류에만 모드 이름을 붙이고 스트림 요청을 제공해요. */
    void test_T5_copter_modes()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        for (const auto type : {MAV_TYPE_QUADROTOR, MAV_TYPE_HEXAROTOR, MAV_TYPE_OCTOROTOR, MAV_TYPE_TRICOPTER,
                               MAV_TYPE_HELICOPTER, MAV_TYPE_COAXIAL, MAV_TYPE_DODECAROTOR, MAV_TYPE_DECAROTOR}) {
            QCOMPARE(plugin->modeName(0, type), QString("STABILIZE"));
            QCOMPARE(plugin->modeName(5, type), QString("LOITER"));
            QCOMPARE(plugin->modeName(4, type), QString("GUIDED"));
            QCOMPARE(plugin->modeName(6, type), QString("RTL"));
        }
        QVERIFY(plugin->modeName(255, MAV_TYPE_QUADROTOR).isEmpty());
        QVERIFY(plugin->modeName(0, MAV_TYPE_FIXED_WING).isEmpty());
        const auto streams = plugin->streamRequests();
        QCOMPARE(streams.size(), 3);
        QCOMPARE(streams[0].streamId, int(MAV_DATA_STREAM_EXTENDED_STATUS));
        QCOMPARE(streams[0].rateHz, 2);
        QCOMPARE(streams[1].streamId, int(MAV_DATA_STREAM_POSITION));
        QCOMPARE(streams[1].rateHz, 4);
        QCOMPARE(streams[2].streamId, int(MAV_DATA_STREAM_EXTRA1));
        QCOMPARE(streams[2].rateHz, 10);
    }
};
QTEST_GUILESS_MAIN(FirmwareTest)
#include "test_firmware.moc"
