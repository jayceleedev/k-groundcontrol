// [0.6.0 §12 1 T6] specs/features/0.6.0-fly.md
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <QtTest>
#include <array>
#include <cmath>
#include <limits>
using kgc::app::geo::AltitudeFrame;
using kgc::vehicle::firmware::CommandForm;
using kgc::vehicle::firmware::CommandStep;
using kgc::vehicle::firmware::FirmwarePlugin;
using kgc::vehicle::firmware::FirmwarePluginFactory;
using kgc::vehicle::firmware::FlightShape;
using kgc::vehicle::firmware::TakeoffKind;
using kgc::vehicle::firmware::VehicleClass;
namespace {
constexpr float kNan = std::numeric_limits<float>::quiet_NaN();
constexpr double kNanDouble = std::numeric_limits<double>::quiet_NaN();
using Modes = QList<std::uint32_t>;
// 기대하는 단계 하나예요. Long은 params 일곱, Int는 앞 넷과 frame·x·y·z를 봐요.
struct Want {
    CommandForm form = CommandForm::Long;
    std::uint16_t command = 0;
    std::array<float, 7> params{};
    std::uint8_t frame = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
    float z = 0.0F;
    Modes confirmModes;
    std::optional<bool> confirmArmed;
};
Want longStep(std::uint16_t command, std::array<float, 7> params, Modes modes = {}, std::optional<bool> armed = {})
{
    return {CommandForm::Long, command, params, 0, 0, 0, 0.0F, modes, armed};
}
Want intStep(std::uint8_t frame, std::array<float, 4> p, std::int32_t x, std::int32_t y, float z, Modes modes)
{
    return {CommandForm::Int, MAV_CMD_DO_REPOSITION, {p[0], p[1], p[2], p[3], 0, 0, 0}, frame, x, y, z, modes, {}};
}
Want mode(float param2, Modes modes, float param3 = 0.0F)
{
    return longStep(MAV_CMD_DO_SET_MODE, {1, param2, param3, 0, 0, 0, 0}, modes);
}
const Want kArm = longStep(MAV_CMD_COMPONENT_ARM_DISARM, {1, 0, 0, 0, 0, 0, 0}, {}, true);
// NaN 칸은 ==로 비교하면 거짓이라 칸마다 isnan으로 봐요.
bool same(float a, float b)
{
    return (std::isnan(a) && std::isnan(b)) || a == b;
}
QString mismatch(const QList<CommandStep>& got, const QList<Want>& want)
{
    if (got.size() != want.size()) return QStringLiteral("단계 수 %1, 기대 %2").arg(got.size()).arg(want.size());
    for (qsizetype i = 0; i < got.size(); ++i) {
        const auto& g = got[i];
        const auto& w = want[i];
        const std::array<float, 7> params{g.param1, g.param2, g.param3, g.param4, g.param5, g.param6, g.param7};
        bool ok = g.form == w.form && g.command == w.command && g.confirmModes == w.confirmModes && g.confirmArmed == w.confirmArmed;
        const int count = w.form == CommandForm::Long ? 7 : 4;
        for (int p = 0; p < count; ++p) ok = ok && same(params[p], w.params[p]);
        if (w.form == CommandForm::Int) ok = ok && g.frame == w.frame && g.x == w.x && g.y == w.y && same(g.z, w.z);
        if (!ok) return QStringLiteral("%1번째 단계(명령 %2)가 달라요").arg(i).arg(g.command);
    }
    return {};
}
#define EXPECT_STEPS(got, ...) do { const auto text = mismatch((got), QList<Want>{__VA_ARGS__}); QVERIFY2(text.isEmpty(), qPrintable(text)); } while (false)
}
class FlyStepsTest : public QObject {
    Q_OBJECT
private slots:
    /** T6: 시동·해제·비상 정지 단계는 세 플러그인이 같아요. */
    void test_T6_common_arm_steps()
    {
        for (const auto autopilot : {std::uint8_t(MAV_AUTOPILOT_ARDUPILOTMEGA), std::uint8_t(MAV_AUTOPILOT_PX4), std::uint8_t(250)}) {
            const auto plugin = FirmwarePluginFactory::create(autopilot);
            EXPECT_STEPS(plugin->armSteps(), kArm);
            EXPECT_STEPS(plugin->disarmSteps(), longStep(MAV_CMD_COMPONENT_ARM_DISARM, {0, 0, 0, 0, 0, 0, 0}, {}, false));
            EXPECT_STEPS(plugin->emergencyStopSteps(), longStep(MAV_CMD_COMPONENT_ARM_DISARM, {0, 21196, 0, 0, 0, 0, 0}, {}, false));
        }
    }
    /** T6: ArduPilot 모드 바꾸기는 쿼드플레인 RTL·QRTL만 확인 모드가 여럿이에요. */
    void test_T6_ardupilot_set_mode()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        EXPECT_STEPS(plugin->setModeSteps(VehicleClass::Multicopter, 5), mode(5, {5}));
        EXPECT_STEPS(plugin->setModeSteps(VehicleClass::Multicopter, 11), mode(11, {11}));
        EXPECT_STEPS(plugin->setModeSteps(VehicleClass::FixedWing, 11), mode(11, {11}));
        EXPECT_STEPS(plugin->setModeSteps(VehicleClass::Vtol, 11), mode(11, {11, 20, 21}));
        EXPECT_STEPS(plugin->setModeSteps(VehicleClass::Vtol, 21), mode(21, {21, 20}));
        EXPECT_STEPS(plugin->setModeSteps(VehicleClass::Vtol, 19), mode(19, {19}));
    }
    /** T6: ArduPilot 이륙은 모드 → (시동) → NAV_TAKEOFF이고, 고정익은 TAKEOFF 모드와 시동뿐이에요. */
    void test_T6_ardupilot_takeoff()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        const auto takeoff = [](float altitude) { return longStep(MAV_CMD_NAV_TAKEOFF, {0, 0, 0, 0, 0, 0, altitude}); };
        EXPECT_STEPS(plugin->takeoffSteps(VehicleClass::Multicopter, 20, false), mode(4, {4}), kArm, takeoff(20));
        EXPECT_STEPS(plugin->takeoffSteps(VehicleClass::Multicopter, 20, true), mode(4, {4}), takeoff(20));
        EXPECT_STEPS(plugin->takeoffSteps(VehicleClass::Vtol, 40, false), mode(15, {15}), kArm, takeoff(40));
        EXPECT_STEPS(plugin->takeoffSteps(VehicleClass::FixedWing, kNanDouble, false), mode(13, {13}), kArm);
        QVERIFY(plugin->takeoffSteps(VehicleClass::Other, 20, false).isEmpty());
    }
    /** T6: ArduPilot 착륙·RTL·일시정지 단계예요. */
    void test_T6_ardupilot_land_rtl_pause()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        EXPECT_STEPS(plugin->landSteps(VehicleClass::Multicopter), mode(9, {9}));
        EXPECT_STEPS(plugin->landSteps(VehicleClass::Vtol), mode(20, {20}));
        QVERIFY(plugin->landSteps(VehicleClass::FixedWing).isEmpty());
        EXPECT_STEPS(plugin->rtlSteps(VehicleClass::Multicopter), mode(6, {6}));
        EXPECT_STEPS(plugin->rtlSteps(VehicleClass::FixedWing), mode(11, {11, 20, 21}));
        EXPECT_STEPS(plugin->rtlSteps(VehicleClass::Vtol), mode(11, {11, 20, 21}));
        EXPECT_STEPS(plugin->pauseSteps(VehicleClass::Multicopter, FlightShape::Hover, ""), mode(17, {17}));
        EXPECT_STEPS(plugin->pauseSteps(VehicleClass::FixedWing, FlightShape::Forward, ""), mode(12, {12}));
        EXPECT_STEPS(plugin->pauseSteps(VehicleClass::Vtol, FlightShape::Hover, ""), mode(19, {19}));
        EXPECT_STEPS(plugin->pauseSteps(VehicleClass::Vtol, FlightShape::Forward, "4.7.2"), mode(12, {12}));
    }
    /** T6: ArduPilot 여기로 가기는 홈 기준 고도 틀의 DO_REPOSITION이고 멀티콥터만 방향 칸이 NaN이에요. */
    void test_T6_ardupilot_go_to()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        EXPECT_STEPS(plugin->goToSteps(VehicleClass::Multicopter, 35.1605, 129.1622, 50),
            intStep(MAV_FRAME_GLOBAL_RELATIVE_ALT_INT, {-1, 1, 0, kNan}, 351605000, 1291622000, 50, {4}));
        EXPECT_STEPS(plugin->goToSteps(VehicleClass::FixedWing, 35.1605, 129.1622, 100),
            intStep(MAV_FRAME_GLOBAL_RELATIVE_ALT_INT, {-1, 1, 0, 0}, 351605000, 1291622000, 100, {15}));
        QVERIFY(plugin->goToSteps(VehicleClass::Other, 35.1605, 129.1622, 50).isEmpty());
    }
    /** T6: ArduPilot의 메시지 주기·이륙 방식·고도 틀·종류 판정·시동 즉시 이륙 모드예요. */
    void test_T6_ardupilot_data()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        const auto intervals = plugin->messageIntervals();
        QCOMPARE(intervals.size(), 1);
        QCOMPARE(intervals[0].messageId, std::uint16_t(245));
        QCOMPARE(intervals[0].intervalUs, 1000000);
        QCOMPARE(plugin->takeoffKind(VehicleClass::Multicopter), TakeoffKind::WithAltitude);
        QCOMPARE(plugin->takeoffKind(VehicleClass::Vtol), TakeoffKind::WithAltitude);
        QCOMPARE(plugin->takeoffKind(VehicleClass::FixedWing), TakeoffKind::FixedAltitude);
        QCOMPARE(plugin->takeoffKind(VehicleClass::Other), TakeoffKind::None);
        QCOMPARE(plugin->takeoffAltitudeParam(), QString("TKOFF_ALT"));
        QCOMPARE(plugin->takeoffAltitudeFrame(VehicleClass::Multicopter), AltitudeFrame::RelativeHome);
        QCOMPARE(plugin->takeoffAltitudeFrame(VehicleClass::Vtol), AltitudeFrame::AboveCurrent);
        QCOMPARE(plugin->takeoffAltitudeFrame(VehicleClass::FixedWing), AltitudeFrame::RelativeHome);
        QCOMPARE(plugin->goToAltitudeFrame(), AltitudeFrame::RelativeHome);
        // §12 고침 1: 고정익과 VTOL 19·20·21만 비행 상태가 필요해요(쿼드·동축·헬리는 ArduCopter로 봐요).
        for (const int type : {1, 19, 20, 21}) QVERIFY2(plugin->vtolStateDecidesClass(type), qPrintable(QString::number(type)));
        for (const int type : {0, 2, 3, 4, 10, 13, 22}) QVERIFY2(!plugin->vtolStateDecidesClass(type), qPrintable(QString::number(type)));
        QCOMPARE(plugin->launchOnArmModes(VehicleClass::FixedWing), Modes({13}));
        QCOMPARE(plugin->launchOnArmModes(VehicleClass::Vtol), Modes({13}));
        QCOMPARE(plugin->launchOnArmModes(VehicleClass::Multicopter), Modes());
        QCOMPARE(plugin->launchOnArmModes(VehicleClass::Other), Modes());
    }
    /** T6: PX4 모드 바꾸기는 종류를 보지 않고 주 모드·보조 모드를 param2·param3에 넣어요. */
    void test_T6_px4_set_mode()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        EXPECT_STEPS(plugin->setModeSteps(VehicleClass::Multicopter, 196608), mode(3, {196608}, 0));
        EXPECT_STEPS(plugin->setModeSteps(VehicleClass::FixedWing, 196608), mode(3, {196608}, 0));
        EXPECT_STEPS(plugin->setModeSteps(VehicleClass::Multicopter, 50593792), mode(4, {50593792}, 3));
    }
    /** T6: PX4 이륙은 NAV_TAKEOFF(해수면 고도)를 먼저, 시동을 나중에 보내요. */
    void test_T6_px4_takeoff()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        const auto takeoff = longStep(MAV_CMD_NAV_TAKEOFF, {kNan, kNan, 0, kNan, kNan, kNan, 23.07F}, {33816576});
        EXPECT_STEPS(plugin->takeoffSteps(VehicleClass::Multicopter, 23.07, false), takeoff, kArm);
        EXPECT_STEPS(plugin->takeoffSteps(VehicleClass::Multicopter, 23.07, true), takeoff);
        QVERIFY(plugin->takeoffSteps(VehicleClass::FixedWing, 23.07, false).isEmpty());
    }
    /** T6: PX4 착륙·RTL과, 1.12 이상에서만 내는 제자리 Hold 일시정지예요. */
    void test_T6_px4_land_rtl_pause()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        EXPECT_STEPS(plugin->landSteps(VehicleClass::Multicopter), mode(4, {100925440}, 6));
        QVERIFY(plugin->landSteps(VehicleClass::FixedWing).isEmpty());
        EXPECT_STEPS(plugin->rtlSteps(VehicleClass::Multicopter), mode(4, {84148224}, 5));
        EXPECT_STEPS(plugin->rtlSteps(VehicleClass::FixedWing), mode(4, {84148224}, 5));
        const auto hold = intStep(MAV_FRAME_GLOBAL_INT, {-1, 1, 0, kNan}, 2147483647, 2147483647, kNan, {50593792});
        EXPECT_STEPS(plugin->pauseSteps(VehicleClass::Multicopter, FlightShape::Hover, "1.15.4"), hold);
        EXPECT_STEPS(plugin->pauseSteps(VehicleClass::Multicopter, FlightShape::Hover, "1.12.0"), hold);
        QVERIFY(plugin->pauseSteps(VehicleClass::Multicopter, FlightShape::Hover, "1.11.3").isEmpty());
        QVERIFY(plugin->pauseSteps(VehicleClass::Multicopter, FlightShape::Hover, "").isEmpty());
    }
    /** T6: PX4 여기로 가기와 메시지 주기·이륙 방식·고도 틀·종류 판정·시동 즉시 이륙 모드예요. */
    void test_T6_px4_go_to_and_data()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4);
        EXPECT_STEPS(plugin->goToSteps(VehicleClass::Multicopter, 35.1605, 129.1622, 53.07),
            intStep(MAV_FRAME_GLOBAL_INT, {-1, 1, 0, kNan}, 351605000, 1291622000, 53.07F, {50593792}));
        QVERIFY(plugin->messageIntervals().isEmpty());
        QCOMPARE(plugin->takeoffKind(VehicleClass::Multicopter), TakeoffKind::WithAltitude);
        QCOMPARE(plugin->takeoffKind(VehicleClass::Vtol), TakeoffKind::WithAltitude);
        QCOMPARE(plugin->takeoffKind(VehicleClass::FixedWing), TakeoffKind::None);
        for (const auto vehicleClass : {VehicleClass::Unknown, VehicleClass::Multicopter, VehicleClass::FixedWing,
                                        VehicleClass::Vtol, VehicleClass::Other})
            QCOMPARE(plugin->takeoffAltitudeFrame(vehicleClass), AltitudeFrame::Amsl);
        QCOMPARE(plugin->goToAltitudeFrame(), AltitudeFrame::Amsl);
        for (const int type : {19, 21, 22, 25}) QVERIFY2(plugin->vtolStateDecidesClass(type), qPrintable(QString::number(type)));
        for (const int type : {1, 2, 13}) QVERIFY2(!plugin->vtolStateDecidesClass(type), qPrintable(QString::number(type)));
        QCOMPARE(plugin->launchOnArmModes(VehicleClass::Multicopter), Modes({33816576}));
        QCOMPARE(plugin->launchOnArmModes(VehicleClass::Vtol), Modes({33816576}));
        QCOMPARE(plugin->launchOnArmModes(VehicleClass::FixedWing), Modes());
    }
    /** T6: 범용 플러그인은 시동·해제·비상 정지 말고는 아무 단계도 내지 않아요. */
    void test_T6_generic()
    {
        const auto plugin = FirmwarePluginFactory::create(250);
        QCOMPARE(plugin->takeoffKind(VehicleClass::Multicopter), TakeoffKind::None);
        QVERIFY(plugin->landSteps(VehicleClass::Multicopter).isEmpty());
        QVERIFY(plugin->rtlSteps(VehicleClass::Multicopter).isEmpty());
        QVERIFY(plugin->pauseSteps(VehicleClass::Multicopter, FlightShape::Hover, "1.15.4").isEmpty());
        QVERIFY(plugin->goToSteps(VehicleClass::Multicopter, 35.1605, 129.1622, 50).isEmpty());
        QVERIFY(plugin->setModeSteps(VehicleClass::Multicopter, 1).isEmpty());
        QVERIFY(plugin->flightModes(VehicleClass::Multicopter).isEmpty());
        for (const auto vehicleClass : {VehicleClass::Unknown, VehicleClass::Multicopter, VehicleClass::FixedWing,
                                        VehicleClass::Vtol, VehicleClass::Other}) {
            QCOMPARE(plugin->takeoffAltitudeFrame(vehicleClass), AltitudeFrame::RelativeHome);
            QVERIFY(plugin->launchOnArmModes(vehicleClass).isEmpty());
        }
        QCOMPARE(plugin->goToAltitudeFrame(), AltitudeFrame::RelativeHome);
        QVERIFY(plugin->vtolStateDecidesClass(MAV_TYPE_FIXED_WING));
        QVERIFY(!plugin->vtolStateDecidesClass(MAV_TYPE_QUADROTOR));
        QVERIFY(!plugin->vtolStateDecidesClass(MAV_TYPE_VTOL_TILTROTOR));
    }
};
QTEST_GUILESS_MAIN(FlyStepsTest)
#include "test_T6_fly_steps.moc"
