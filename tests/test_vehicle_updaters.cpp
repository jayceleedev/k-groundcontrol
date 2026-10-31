// [0.1.0 T2] specs/features/0.1.0-link.md
#include "updaters/LinkUpdater.h"
#include "updaters/ArmingUpdater.h"
#include "updaters/AttitudeUpdater.h"
#include "updaters/PositionUpdater.h"
#include "updaters/GpsUpdater.h"
#include "updaters/BatteryUpdater.h"
#include "updaters/CapabilitiesUpdater.h"
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <QtTest>
#include <cmath>
#include <numbers>
using namespace kgc::vehicle::states;
class VehicleUpdatersTest : public QObject {
    Q_OBJECT
private slots:
    /** T2: 수신 전에는 숫자를 실제 영으로 오인하지 않아요. */
    void test_T2_initial_unknowns()
    {
        const PositionBundle position;
        QVERIFY(std::isnan(position.latDeg) && std::isnan(position.lonDeg));
        QVERIFY(std::isnan(position.altAmslM) && std::isnan(position.altRelM));
        QVERIFY(std::isnan(position.groundSpeedMs) && std::isnan(position.climbMs) && std::isnan(position.headingDeg));
        const AttitudeBundle attitude;
        QVERIFY(std::isnan(attitude.rollDeg) && std::isnan(attitude.pitchDeg) && std::isnan(attitude.yawDeg));
        QCOMPARE(LinkBundle{}.lastHeartbeatMs, -1);
        QVERIFY(!LinkBundle{}.connected && !LinkBundle{}.lost);
        QCOMPARE(ArmingBundle{}.modeId, -1);
        QCOMPARE(CapabilitiesBundle{}.vehicleType, -1);
        const GpsBundle gps;
        QCOMPARE(gps.fixType, -1);
        QCOMPARE(gps.satellites, -1);
        QVERIFY(std::isnan(gps.hdop));
        const BatteryBundle battery;
        QVERIFY(std::isnan(battery.voltageV) && std::isnan(battery.currentA));
        QCOMPARE(battery.remainingPct, -1);
    }
    /** T2: 하트비트는 링크와 시동, 모드, 펌웨어 묶음을 채워요. */
    void test_T2_heartbeat()
    {
        mavlink_message_t message{};
        mavlink_msg_heartbeat_pack(1, 1, &message, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_PX4,
                                  MAV_MODE_FLAG_SAFETY_ARMED, 3u << 16, MAV_STATE_ACTIVE);
        auto plugin = kgc::vehicle::firmware::FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        const auto link = updateLink(message, 4567);
        QVERIFY(link.connected && !link.lost);
        QCOMPARE(link.lastHeartbeatMs, 4567);
        const auto arming = updateArming(message, *plugin);
        QVERIFY(arming.armed);
        QCOMPARE(arming.modeId, 3u << 16);
        QCOMPARE(arming.modeName, QString("Position"));
        const auto capabilities = updateCapabilities(message, *plugin);
        QCOMPARE(capabilities.vehicleType, int(MAV_TYPE_QUADROTOR));
        QCOMPARE(capabilities.firmwareName, QString("PX4"));
        mavlink_msg_heartbeat_pack(1, 1, &message, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_PX4,
                                  0, 3u << 16, MAV_STATE_ACTIVE);
        QVERIFY(!updateArming(message, *plugin).armed);
    }
    /** T2: 라디안을 도로 바꾸고 음수 방위를 한 바퀴 안으로 옮겨요. */
    void test_T2_attitude()
    {
        mavlink_message_t message{};
        mavlink_attitude_t input{};
        input.roll = std::numbers::pi_v<float> / 6;
        input.pitch = -std::numbers::pi_v<float> / 4;
        input.yaw = -std::numbers::pi_v<float> / 2;
        mavlink_msg_attitude_encode(1, 1, &message, &input);
        const auto value = updateAttitude(message);
        QVERIFY(std::abs(value.rollDeg - 30) < 0.0001);
        QVERIFY(std::abs(value.pitchDeg + 45) < 0.0001);
        QVERIFY(std::abs(value.yawDeg - 270) < 0.0001);
        input.yaw = std::numbers::pi_v<float> / 2;
        mavlink_msg_attitude_encode(1, 1, &message, &input);
        QVERIFY(std::abs(updateAttitude(message).yawDeg - 90) < 0.0001);
    }
    /** T2: 위치 단위와 상승 부호를 바꾸고 없는 헤딩을 보존해요. */
    void test_T2_position()
    {
        mavlink_global_position_int_t input{};
        input.lat = 371234567;
        input.lon = 1271234567;
        input.alt = 123450;
        input.relative_alt = -1500;
        input.vx = 300;
        input.vy = 400;
        input.vz = -250;
        input.hdg = 12345;
        mavlink_message_t message{};
        mavlink_msg_global_position_int_encode(1, 1, &message, &input);
        const auto value = updatePosition(message);
        QCOMPARE(value.latDeg, 37.1234567);
        QCOMPARE(value.lonDeg, 127.1234567);
        QCOMPARE(value.altAmslM, 123.45);
        QCOMPARE(value.altRelM, -1.5);
        QCOMPARE(value.groundSpeedMs, 5.0);
        QCOMPARE(value.climbMs, 2.5);
        QCOMPARE(value.headingDeg, 123.45);
        input.hdg = UINT16_MAX;
        mavlink_msg_global_position_int_encode(1, 1, &message, &input);
        QVERIFY(std::isnan(updatePosition(message).headingDeg));
    }
    /** T2: GPS의 위성과 정밀도 미확인 값을 실제 값과 구분해요. */
    void test_T2_gps()
    {
        mavlink_gps_raw_int_t input{};
        input.fix_type = 3;
        input.satellites_visible = 12;
        input.eph = 125;
        mavlink_message_t message{};
        mavlink_msg_gps_raw_int_encode(1, 1, &message, &input);
        const auto value = updateGps(message);
        QCOMPARE(value.fixType, 3);
        QCOMPARE(value.satellites, 12);
        QCOMPARE(value.hdop, 1.25);
        input.satellites_visible = UINT8_MAX;
        input.eph = UINT16_MAX;
        mavlink_msg_gps_raw_int_encode(1, 1, &message, &input);
        QCOMPARE(updateGps(message).satellites, -1);
        QVERIFY(std::isnan(updateGps(message).hdop));
    }
    /** T2: 전압과 전류 단위를 바꾸고 배터리 미확인 값을 보존해요. */
    void test_T2_battery()
    {
        mavlink_sys_status_t input{};
        input.voltage_battery = 12345;
        input.current_battery = 125;
        input.battery_remaining = 73;
        mavlink_message_t message{};
        mavlink_msg_sys_status_encode(1, 1, &message, &input);
        const auto value = updateBattery(message);
        QCOMPARE(value.voltageV, 12.345);
        QCOMPARE(value.currentA, 1.25);
        QCOMPARE(value.remainingPct, 73);
        input.voltage_battery = UINT16_MAX;
        input.current_battery = -1;
        input.battery_remaining = -1;
        mavlink_msg_sys_status_encode(1, 1, &message, &input);
        const auto missing = updateBattery(message);
        QVERIFY(std::isnan(missing.voltageV) && std::isnan(missing.currentA));
        QCOMPARE(missing.remainingPct, -1);
    }
};
QTEST_GUILESS_MAIN(VehicleUpdatersTest)
#include "test_vehicle_updaters.moc"
