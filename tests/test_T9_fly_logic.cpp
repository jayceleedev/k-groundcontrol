// [0.6.0 T9] specs/features/0.6.0-fly.md
#include "FlyChecks.h"
#include "FlyText.h"
#include "StepPacking.h"
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <QtTest>
#include <cmath>
#include <limits>
using namespace kgc::fly;
using kgc::vehicle::firmware::FirmwarePluginFactory;
using kgc::vehicle::firmware::FlightMode;
using kgc::vehicle::firmware::TakeoffKind;
using kgc::vehicle::firmware::VehicleClass;
using kgc::vehicle::states::VehicleMessage;
namespace {
constexpr double kNan = std::numeric_limits<double>::quiet_NaN();
// T9 표의 바탕: AP 멀티콥터 능력, 링크 있음, 위치·홈 앎, 실행 중 아님, 모드 0이에요.
FlyInputs base()
{
    const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
    FlyInputs in;
    in.capabilities.vehicleClass = VehicleClass::Multicopter;
    in.capabilities.classKnown = true;
    in.capabilities.takeoff = TakeoffKind::WithAltitude;
    in.capabilities.land = in.capabilities.rtl = in.capabilities.pause = in.capabilities.goTo = true;
    in.capabilities.modes = plugin->flightModes(VehicleClass::Multicopter);
    in.linkOk = true;
    in.hasPosition = in.hasHome = true;
    in.modeId = 0;
    return in;
}
// T9 14·15행의 AP 고정익 능력이에요.
FlyCapabilities fixedWing()
{
    const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
    FlyCapabilities caps;
    caps.vehicleClass = VehicleClass::FixedWing;
    caps.takeoff = TakeoffKind::FixedAltitude;
    caps.rtl = caps.pause = caps.goTo = true;
    caps.modes = plugin->flightModes(VehicleClass::FixedWing);
    caps.launchOnArmModes = plugin->launchOnArmModes(VehicleClass::FixedWing);
    return caps;
}
// 표의 한 행: arm, disarm, eStop, takeoff, land, rtl, pause, setMode, goTo 차례예요.
FlyReadiness row(bool arm, bool disarm, bool stop, bool takeoff, bool land, bool rtl, bool pause, bool setMode, bool goTo)
{
    return {arm, disarm, stop, takeoff, land, rtl, pause, setMode, goTo};
}
FlyInputs with(bool armed, int landed)
{
    auto in = base();
    in.armed = armed;
    in.landedState = landed;
    return in;
}
}
class FlyLogicTest : public QObject {
    Q_OBJECT
private slots:
    /** T9: 바탕 능력으로 시동·착륙 상태만 바꾼 1~6행의 누를 수 있는 단추가 표와 같아요. */
    void test_T9_readiness_rows_1_to_6()
    {
        QCOMPARE(base().capabilities.modes.size(), 14);
        QCOMPARE(readinessOf(with(false, 1)), row(true, false, false, true, false, false, false, true, false));
        QCOMPARE(readinessOf(with(true, 1)), row(false, true, false, true, false, false, false, true, false));
        QCOMPARE(readinessOf(with(true, 2)), row(false, false, true, false, true, true, true, true, true));
        QCOMPARE(readinessOf(with(true, 3)), row(false, false, true, false, true, true, true, true, false));
        QCOMPARE(readinessOf(with(true, -1)), row(false, true, true, false, true, true, true, true, false));
        QCOMPARE(readinessOf(with(false, -1)), row(true, false, false, true, false, false, false, true, false));
    }
    /** T9: 실행 중·링크 없음·홈 모름·범용 능력·종류 모름(7~13행)이 표와 같아요. */
    void test_T9_readiness_rows_7_to_13()
    {
        auto busy = with(true, 2);
        busy.busy = true;
        QCOMPARE(readinessOf(busy), row(false, false, true, false, false, false, false, false, false));
        auto noLink = with(true, 2);
        noLink.linkOk = false;
        QCOMPARE(readinessOf(noLink), FlyReadiness{});
        auto noHome = with(true, 2);
        noHome.hasHome = false;
        QCOMPARE(readinessOf(noHome), row(false, false, true, false, true, true, true, true, false));
        auto generic = with(true, 2);
        generic.capabilities = FlyCapabilities{};
        QCOMPARE(generic.capabilities.modes.size(), 0);
        QCOMPARE(readinessOf(generic), row(false, false, true, false, false, false, false, false, false));
        auto unknownArmed = with(true, -1);
        unknownArmed.capabilities.classKnown = false;
        QCOMPARE(readinessOf(unknownArmed), row(false, true, true, false, false, false, false, false, false));
        auto unknownDisarmed = with(false, -1);
        unknownDisarmed.capabilities.classKnown = false;
        QCOMPARE(readinessOf(unknownDisarmed), row(true, false, false, false, false, false, false, false, false));
        auto busyGround = with(true, 1);
        busyGround.busy = true;
        QCOMPARE(readinessOf(busyGround), row(false, true, false, false, false, false, false, false, false));
    }
    /** T9: AP 고정익이 TAKEOFF(13) 모드면 '시동'이 흐리고 이륙은 눌리며, FBWA(5)면 둘 다 눌려요(14·15행). */
    void test_T9_readiness_launch_mode()
    {
        auto takeoffMode = with(false, 1);
        takeoffMode.capabilities = fixedWing();
        takeoffMode.modeId = 13;
        QCOMPARE(takeoffMode.capabilities.modes.size(), 12);
        QCOMPARE(takeoffMode.capabilities.launchOnArmModes, QList<std::uint32_t>{13});
        QCOMPARE(readinessOf(takeoffMode), row(false, false, false, true, false, false, false, true, false));
        auto fbwa = takeoffMode;
        fbwa.modeId = 5;
        QCOMPARE(readinessOf(fbwa), row(true, false, false, true, false, false, false, true, false));
    }
    /** T9: 스틱 모드는 땅(1)에서만 고를 수 있고, 그 밖 모드는 늘 고를 수 있어요. */
    void test_T9_mode_choosable()
    {
        const FlightMode stabilize{0, QStringLiteral("STABILIZE"), true};
        QVERIFY(modeChoosable(stabilize, 1));
        for (int landed : {2, 3, 4, 0, -1}) QVERIFY(!modeChoosable(stabilize, landed));
        const FlightMode guided{4, QStringLiteral("GUIDED"), false};
        QVERIFY(modeChoosable(guided, 2));
        QVERIFY(modeChoosable(guided, -1));
    }
    /** T9: ACK 결과와 큐 실패가 비행 오류로, 기체 메시지가 실패 까닭으로 옮겨져요. */
    void test_T9_error_of_and_reason()
    {
        QCOMPARE(errorOf(MAV_RESULT_DENIED), FlyError::Denied);
        QCOMPARE(errorOf(MAV_RESULT_UNSUPPORTED), FlyError::Unsupported);
        QCOMPARE(errorOf(MAV_RESULT_TEMPORARILY_REJECTED), FlyError::TemporarilyRejected);
        QCOMPARE(errorOf(MAV_RESULT_FAILED), FlyError::Failed);
        QCOMPARE(errorOf(MAV_RESULT_CANCELLED), FlyError::Failed);
        using kgc::vehicle::command::CommandFailure;
        QCOMPARE(errorOf(CommandFailure::Timeout), FlyError::NoAck);
        QCOMPARE(errorOf(CommandFailure::Gone), FlyError::Gone);
        QCOMPARE(errorOf(CommandFailure::NoLink), FlyError::NoLink);
        QCOMPARE(errorOf(CommandFailure::Preempted), FlyError::Preempted);
        QVERIFY(isReason(VehicleMessage{4, QStringLiteral("x")}));
        QVERIFY(isReason(VehicleMessage{2, QStringLiteral("x")}));
        QVERIFY(isReason(VehicleMessage{5, QStringLiteral("x")}));
        QVERIFY(!isReason(VehicleMessage{6, QStringLiteral("x")}));
        QVERIFY(!isReason(VehicleMessage{2, QString{}}));
    }
    /** T9: 시동 단계는 Arm, 그 밖은 Motion으로 보내고, 단계 칸이 큐의 명령으로 그대로 옮겨져요. */
    void test_T9_step_packing()
    {
        using kgc::vehicle::outbound::OutboundKind;
        kgc::vehicle::firmware::CommandStep step;
        for (auto [command, kind] : {std::pair{MAV_CMD_COMPONENT_ARM_DISARM, OutboundKind::Arm},
                                     std::pair{MAV_CMD_DO_SET_MODE, OutboundKind::Motion},
                                     std::pair{MAV_CMD_DO_REPOSITION, OutboundKind::Motion},
                                     std::pair{MAV_CMD_NAV_TAKEOFF, OutboundKind::Motion}}) {
            step.command = command;
            QCOMPARE(outboundKindOf(step), kind);
        }
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        const auto goTo = plugin->goToSteps(VehicleClass::Multicopter, 35.1605, 129.1622, 50).at(0);
        const auto packed = toCommandInt(goTo);
        QCOMPARE(packed.command, std::uint16_t(MAV_CMD_DO_REPOSITION));
        QCOMPARE(packed.frame, std::uint8_t(MAV_FRAME_GLOBAL_RELATIVE_ALT_INT));
        QCOMPARE(packed.param1, -1.0F);
        QCOMPARE(packed.param2, 1.0F);
        QCOMPARE(packed.param3, 0.0F);
        QVERIFY(std::isnan(packed.param4));
        QCOMPARE(packed.x, 351605000);
        QCOMPARE(packed.y, 1291622000);
        QCOMPARE(packed.z, 50.0F);
        QCOMPARE(packed.targetCompid, std::uint8_t(1));
        const auto takeoff = plugin->takeoffSteps(VehicleClass::Multicopter, 20, false).at(2);
        const auto longCommand = toCommandLong(takeoff);
        QCOMPARE(longCommand.command, std::uint16_t(MAV_CMD_NAV_TAKEOFF));
        QCOMPARE(longCommand.param7, 20.0F);
        QCOMPARE(longCommand.targetCompid, std::uint8_t(1));
    }
    /** T9: 거리·길이·좌표·동작 이름 글이 정한 모양이고, 모든 오류에 화면 글이 있어요. */
    void test_T9_texts()
    {
        QCOMPARE(distanceText(258.5), QStringLiteral("259 m"));
        QCOMPARE(distanceText(999.4), QStringLiteral("999 m"));
        QCOMPARE(distanceText(999.6), QStringLiteral("1 km"));
        QCOMPARE(distanceText(1234), QStringLiteral("1.2 km"));
        QCOMPARE(distanceText(25000), QStringLiteral("25 km"));
        QCOMPARE(distanceText(kNan), QStringLiteral("—"));
        QCOMPARE(lengthNumber(50), QStringLiteral("50"));
        QCOMPARE(lengthNumber(30.46), QStringLiteral("30.5"));
        QCOMPARE(lengthNumber(30.04), QStringLiteral("30"));
        QCOMPARE(lengthNumber(121.92), QStringLiteral("121.9"));
        QCOMPARE(positionText(35.1587, 129.1604), QStringLiteral("35.158700, 129.160400"));
        QCOMPARE(actionName(FlyActionKind::GoTo), QStringLiteral("GoTo"));
        for (int code = int(FlyError::NoLink); code <= int(FlyError::TargetWithoutMode); ++code)
            QVERIFY2(!errorText(make(FlyError(code))).isEmpty(), qPrintable(flyErrorName(FlyError(code))));
    }
};
QTEST_GUILESS_MAIN(FlyLogicTest)
#include "test_T9_fly_logic.moc"
