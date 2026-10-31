// [0.2.0 T11] specs/features/0.2.0-fcflash.md
#include "updaters/AutopilotVersionUpdater.h"
#include <kgc/app/scheduler/Scheduler.h>
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <kgc/vehicle/states/VehicleStateOwner.h>
#include <kgc/vehicle/states/VehicleStates.h>
#include <QtTest>
using namespace kgc::vehicle::states;
namespace {
mavlink_message_t autopilotVersion(std::uint32_t flightSwVersion)
{
    std::uint8_t flightCustom[8]{};
    std::uint8_t middlewareCustom[8]{};
    std::uint8_t osCustom[8]{};
    std::uint8_t uid2[18]{};
    mavlink_message_t message{};
    mavlink_msg_autopilot_version_pack(1, 1, &message, 0, flightSwVersion, 0, 0, 0, flightCustom, middlewareCustom,
                                       osCustom, 0, 0, 0, uid2);
    return message;
}
mavlink_message_t heartbeat()
{
    mavlink_message_t message{};
    mavlink_msg_heartbeat_pack(1, 1, &message, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_PX4, 0, 0, MAV_STATE_STANDBY);
    return message;
}
}
class AutopilotVersionTest : public QObject {
    Q_OBJECT
private slots:
    /** T11: flight_sw_version의 윗 세 바이트를 major.minor.patch로 풀어요(KOA FC의 실제 값). */
    void test_T11_decodes_version()
    {
        QCOMPARE(updateAutopilotVersion(autopilotVersion(0x011000c0), {}).firmwareVersion, QString("1.16.0"));
        QCOMPARE(updateAutopilotVersion(autopilotVersion(0x040702ff), {}).firmwareVersion, QString("4.7.2"));
    }
    /** T11: 버전을 넣어도 묶음의 다른 값은 그대로예요. */
    void test_T11_keeps_other_fields()
    {
        CapabilitiesBundle current;
        current.vehicleType = MAV_TYPE_QUADROTOR;
        current.firmwareName = "PX4";
        const auto updated = updateAutopilotVersion(autopilotVersion(0x011000c0), current);
        QCOMPARE(updated.vehicleType, int(MAV_TYPE_QUADROTOR));
        QCOMPARE(updated.firmwareName, QString("PX4"));
        QCOMPARE(updated.firmwareVersion, QString("1.16.0"));
    }
    /** T11: 수신 전 값은 빈 문자열이에요. */
    void test_T11_unknown_is_empty()
    {
        QVERIFY(CapabilitiesBundle{}.firmwareVersion.isEmpty());
    }
    /** T11: 상태 주인을 거쳐 AUTOPILOT_VERSION을 받으면 버전이 채워지고 변경을 알려요. */
    void test_T11_owner_applies_version()
    {
        kgc::app::scheduler::Scheduler scheduler;
        VehicleStates states;
        const auto plugin = kgc::vehicle::firmware::FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        VehicleStateOwner owner(states, *plugin, scheduler, 1);
        QSignalSpy changed(&states, &VehicleStates::capabilitiesChanged);
        owner.consume(autopilotVersion(0x011000c0));
        QCOMPARE(states.capabilities().firmwareVersion, QString("1.16.0"));
        QCOMPARE(changed.count(), 1);
    }
    /** T11: HEARTBEAT 갱신 뒤에도 버전이 지워지지 않고, HEARTBEAT 값은 들어와요. */
    void test_T11_version_survives_heartbeat()
    {
        kgc::app::scheduler::Scheduler scheduler;
        VehicleStates states;
        const auto plugin = kgc::vehicle::firmware::FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        VehicleStateOwner owner(states, *plugin, scheduler, 1);
        owner.consume(heartbeat());
        owner.consume(autopilotVersion(0x011000c0));
        owner.consume(heartbeat());
        QCOMPARE(states.capabilities().firmwareVersion, QString("1.16.0"));
        QCOMPARE(states.capabilities().vehicleType, int(MAV_TYPE_QUADROTOR));
        QCOMPARE(states.capabilities().firmwareName, plugin->displayName());
    }
};
QTEST_GUILESS_MAIN(AutopilotVersionTest)
#include "test_autopilot_version.moc"
