// [0.6.0 §12 3 T2] specs/features/0.6.0-fly.md
#include "updaters/FlightUpdater.h"
#include "updaters/PositionUpdater.h"
#include "updaters/NavTargetUpdater.h"
#include "updaters/StatusTextUpdater.h"
#include <QtTest>
#include <cmath>
using namespace std::chrono_literals;
using namespace kgc::vehicle::states;
namespace {
mavlink_message_t targetMessage(std::int32_t latInt, std::int32_t lonInt, float alt, std::uint16_t typeMask)
{
    mavlink_message_t message{};
    mavlink_msg_position_target_global_int_pack(1, 1, &message, 0, MAV_FRAME_GLOBAL_RELATIVE_ALT_INT, typeMask,
        latInt, lonInt, alt, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F);
    return message;
}
mavlink_message_t orbitMessage(float radius, std::uint8_t frame, std::int32_t x, std::int32_t y)
{
    mavlink_message_t message{};
    mavlink_msg_orbit_execution_status_pack(1, 1, &message, 0, radius, frame, x, y, 0.0F);
    return message;
}
}
class NavUpdatersTest : public QObject {
    Q_OBJECT
private slots:
    /** T2: EXTENDED_SYS_STATE의 착륙 상태·VTOL 상태를 그대로 옮기고, 받기 전 묶음은 둘 다 -1이에요. */
    void test_T2_flight()
    {
        mavlink_message_t message{};
        mavlink_msg_extended_sys_state_pack(1, 1, &message, MAV_VTOL_STATE_FW, MAV_LANDED_STATE_IN_AIR);
        const auto flight = updateFlight(message);
        QCOMPARE(flight.vtolState, 4);
        QCOMPARE(flight.landedState, 2);
        QCOMPARE(FlightBundle{}.landedState, -1);
        QCOMPARE(FlightBundle{}.vtolState, -1);
    }
    /** T2: 목표 위치를 도로 바꾸고, 무시 비트나 (0, 0)이면 모르는 위치로 두며 선회 셋은 건드리지 않아요. */
    void test_T2_nav_target()
    {
        NavTargetBundle start;
        start.orbitRadiusM = 80.0;
        start.orbitLatDeg = 35.0;
        start.orbitLonDeg = 129.0;
        const auto target = updateNavTarget(targetMessage(351600000, 1291610000, 33.07F, 0), start);
        QVERIFY(std::abs(target.latDeg - 35.16) <= 1e-9);
        QVERIFY(std::abs(target.lonDeg - 129.161) <= 1e-9);
        QVERIFY(std::abs(target.altM - 33.07) <= 1e-4);
        QCOMPARE(target.orbitRadiusM, 80.0);
        QCOMPARE(target.orbitLatDeg, 35.0);
        QCOMPARE(target.orbitLonDeg, 129.0);
        const auto ignored = updateNavTarget(targetMessage(351600000, 1291610000, 33.07F, 3), start);
        QVERIFY(std::isnan(ignored.latDeg) && std::isnan(ignored.lonDeg));
        const auto zero = updateNavTarget(targetMessage(0, 0, 33.07F, 0), start);
        QVERIFY(std::isnan(zero.latDeg) && std::isnan(zero.lonDeg));
    }
    /** T2: 목표 메시지를 받을 때마다 targetCount가 1 늘고, 선회 메시지로는 그대로예요. */
    void test_T2_target_count()
    {
        auto bundle = updateNavTarget(targetMessage(351600000, 1291610000, 33.07F, 0), NavTargetBundle{});
        bundle = updateNavTarget(targetMessage(351600000, 1291610000, 33.07F, 0), bundle);
        QCOMPARE(bundle.targetCount, 2);
        bundle = updateOrbit(orbitMessage(80.0F, MAV_FRAME_GLOBAL_INT, 351600000, 1291610000), bundle);
        QCOMPARE(bundle.targetCount, 2);
        QCOMPARE(NavTargetBundle{}.targetCount, 0);
    }
    /** T2: 선회 반지름은 부호 그대로, 전역 프레임이면 중심을 도로 바꾸고 목표 셋은 그대로 둬요. */
    void test_T2_orbit()
    {
        NavTargetBundle start;
        start.latDeg = 1.0;
        start.lonDeg = 2.0;
        start.altM = 3.0;
        const auto counter = updateOrbit(orbitMessage(-80.0F, MAV_FRAME_GLOBAL_INT, 351600000, 1291610000), start);
        QCOMPARE(counter.orbitRadiusM, -80.0);
        QVERIFY(std::abs(counter.orbitLatDeg - 35.16) <= 1e-9);
        QVERIFY(std::abs(counter.orbitLonDeg - 129.161) <= 1e-9);
        QCOMPARE(counter.latDeg, 1.0);
        QCOMPARE(counter.lonDeg, 2.0);
        QCOMPARE(counter.altM, 3.0);
        const auto px4 = updateOrbit(orbitMessage(80.0F, MAV_FRAME_GLOBAL, 351600000, 1291610000), start);
        QCOMPARE(px4.orbitRadiusM, 80.0);
        QVERIFY(std::abs(px4.orbitLatDeg - 35.16) <= 1e-9);
        QVERIFY(std::abs(px4.orbitLonDeg - 129.161) <= 1e-9);
        const auto none = updateOrbit(orbitMessage(0.0F, MAV_FRAME_GLOBAL_INT, 351600000, 1291610000), start);
        QVERIFY(std::isnan(none.orbitRadiusM));
        const auto local = updateOrbit(orbitMessage(80.0F, MAV_FRAME_LOCAL_NED, 351600000, 1291610000), start);
        QVERIFY(std::isnan(local.orbitLatDeg) && std::isnan(local.orbitLonDeg));
        QCOMPARE(local.orbitRadiusM, 80.0);
    }
    /** T2: 한도보다 오래 지났을 때만 만료예요(같으면 아직 살아 있어요). */
    void test_T2_is_stale()
    {
        QVERIFY(isStale(1000, 4001, 3000ms));
        QVERIFY(!isStale(1000, 4000, 3000ms));
        QVERIFY(isStale(1000, 6001, 5000ms));
        QVERIFY(!isStale(1000, 6000, 5000ms));
    }
    /** T2: STATUSTEXT를 심각도와 글로 풀고, 끝 NUL이 없는 꽉 찬 50바이트도 50자로 읽어요. */
    void test_T2_status_text()
    {
        // pack은 칸 크기(50바이트)만큼 읽으므로 글을 칸 크기 버퍼에 담아 넘겨요.
        char buffer[MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN] = {};
        qstrncpy(buffer, "PreArm: Throttle too high", sizeof(buffer));
        mavlink_message_t message{};
        mavlink_msg_statustext_pack(1, 1, &message, MAV_SEVERITY_WARNING, buffer, 0, 0);
        const auto text = statusTextOf(message);
        QCOMPARE(text.severity, 4);
        QCOMPARE(text.text, QString("PreArm: Throttle too high"));
        char full[MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN];
        for (auto& c : full) c = 'x';
        mavlink_message_t fullMessage{};
        mavlink_msg_statustext_pack(1, 1, &fullMessage, MAV_SEVERITY_INFO, full, 0, 0);
        const auto fullText = statusTextOf(fullMessage);
        QCOMPARE(fullText.text.size(), 50);
        QCOMPARE(fullText.text, QString(50, QChar('x')));
    }
    /** T2(§12 고침 3): GPS 고정 전 (0, 0) 근처 위치는 3D fix나 홈을 알 때까지 위도·경도만 버리고 나머지는 그대로 둬요. */
    void test_T2_gate_position()
    {
        PositionBundle early;
        early.latDeg = 0.0002982;
        early.lonDeg = 0.0000568;
        early.altRelM = 0.2;
        const auto noFix = gatePosition(early, 1);
        QVERIFY(std::isnan(noFix.latDeg));
        QVERIFY(std::isnan(noFix.lonDeg));
        QCOMPARE(noFix.altRelM, 0.2);
        const auto unknownFix = gatePosition(early, -1);
        QVERIFY(std::isnan(unknownFix.latDeg));
        QVERIFY(std::isnan(unknownFix.lonDeg));
        const auto fixed = gatePosition(early, 3);
        QCOMPARE(fixed.latDeg, 0.0002982);
        QCOMPARE(fixed.lonDeg, 0.0000568);
        PositionBundle withHome;
        withHome.latDeg = 35.1587;
        withHome.lonDeg = 129.1604;
        withHome.homeLatDeg = 35.1587;
        withHome.homeLonDeg = 129.1604;
        const auto kept = gatePosition(withHome, 0);
        QCOMPARE(kept.latDeg, 35.1587);
        QCOMPARE(kept.lonDeg, 129.1604);
    }
};
QTEST_GUILESS_MAIN(NavUpdatersTest)
#include "test_T2_nav_updaters.moc"
