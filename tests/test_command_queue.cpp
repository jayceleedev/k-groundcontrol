// [0.2.0 T6] specs/features/0.2.0-fcflash.md
#include <kgc/vehicle/command/CommandQueue.h>
#include <kgc/vehicle/outbound/Errors.h>
#include <QtTest>
using namespace std::chrono_literals;
using kgc::app::errors::Result;
using kgc::vehicle::command::CommandFailure;
using kgc::vehicle::command::CommandLong;
using kgc::vehicle::command::CommandOptions;
using kgc::vehicle::command::CommandQueue;
using kgc::vehicle::command::CommandResult;
using kgc::vehicle::outbound::OutboundError;
using kgc::vehicle::outbound::OutboundKind;
using kgc::vehicle::outbound::VehicleSender;
namespace {
constexpr int kVehicleSysid = 7;
constexpr std::uint8_t kGcsSysid = 255;
constexpr std::uint8_t kGcsCompid = 190;
constexpr std::uint8_t kChannel = 0;
/** 큐가 보낸 프레임을 풀어 기록하고, 정해진 결과를 돌려주는 가짜 송신 함수예요. */
class FakeSender {
public:
    QList<mavlink_command_long_t> sent;
    QList<OutboundKind> kinds;
    bool linkUp = true;
    CommandQueue::SendFn fn()
    {
        return [this](OutboundKind kind, VehicleSender::PackFn pack) -> Result<void> {
            kinds << kind;
            if (!linkUp) return std::unexpected(kgc::vehicle::outbound::make(OutboundError::NoLink));
            mavlink_message_t message{};
            pack(kGcsSysid, kGcsCompid, kChannel, message);
            mavlink_command_long_t command{};
            mavlink_msg_command_long_decode(&message, &command);
            sent << command;
            return {};
        };
    }
};
CommandLong rebootCommand()
{
    CommandLong command;
    command.command = MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN;
    command.param1 = 3.0F;
    return command;
}
CommandLong requestCommand()
{
    CommandLong command;
    command.command = MAV_CMD_REQUEST_MESSAGE;
    command.param1 = 148.0F;
    return command;
}
}
class CommandQueueTest : public QObject {
    Q_OBJECT
private slots:
    /** T6: 값과 대상, 송신 종류가 COMMAND_LONG에 그대로 담겨요. */
    void test_T6_sends_command_fields()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        auto command = rebootCommand();
        command.param7 = 2.5F;
        command.targetCompid = 5;
        queue.send(command, {}, [](CommandResult) {}, this);
        QTRY_COMPARE_WITH_TIMEOUT(sender.sent.size(), 1, 500);
        const auto& frame = sender.sent.first();
        QCOMPARE(frame.command, std::uint16_t(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN));
        QCOMPARE(frame.param1, 3.0F);
        QCOMPARE(frame.param7, 2.5F);
        QCOMPARE(frame.target_system, std::uint8_t(kVehicleSysid));
        QCOMPARE(frame.target_component, std::uint8_t(5));
        QCOMPARE(frame.confirmation, std::uint8_t(0));
        QCOMPARE(sender.kinds.first(), OutboundKind::Command);
    }
    /** T6: 기본 대상 컴포넌트는 1이고 기본 시간은 1200ms·3회예요. */
    void test_T6_defaults()
    {
        QCOMPARE(CommandLong{}.targetCompid, std::uint8_t(1));
        QCOMPARE(CommandOptions{}.timeout, 1200ms);
        QCOMPARE(CommandOptions{}.retries, 3);
    }
    /** T6: 같은 번호의 ACK가 오면 그 결과로 한 번 끝나요. */
    void test_T6_ack_success()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        int done = 0;
        CommandResult last;
        queue.send(rebootCommand(), {}, [&](CommandResult result) { ++done; last = result; }, this);
        QTRY_COMPARE_WITH_TIMEOUT(sender.sent.size(), 1, 500);
        queue.receiveAck(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN, MAV_RESULT_ACCEPTED);
        QTRY_COMPARE_WITH_TIMEOUT(done, 1, 500);
        QVERIFY(last.result == MAV_RESULT_ACCEPTED);
        QVERIFY(!last.failure.has_value());
    }
    /** T6: 거부 ACK도 재시도 없이 그 결과로 끝나요. */
    void test_T6_denied_ack_is_final()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        CommandResult last;
        int done = 0;
        queue.send(rebootCommand(), {60ms, 3}, [&](CommandResult result) { ++done; last = result; }, this);
        QTRY_COMPARE_WITH_TIMEOUT(sender.sent.size(), 1, 500);
        queue.receiveAck(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN, MAV_RESULT_DENIED);
        QTRY_COMPARE_WITH_TIMEOUT(done, 1, 500);
        QVERIFY(last.result == MAV_RESULT_DENIED);
        QTest::qWait(150);
        QCOMPARE(sender.sent.size(), 1);
    }
    /** T6: 다른 번호의 ACK는 무시하고 계속 기다려요. */
    void test_T6_other_command_ack_ignored()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        int done = 0;
        queue.send(rebootCommand(), {}, [&](CommandResult) { ++done; }, this);
        QTRY_COMPARE_WITH_TIMEOUT(sender.sent.size(), 1, 500);
        queue.receiveAck(MAV_CMD_REQUEST_MESSAGE, MAV_RESULT_ACCEPTED);
        QTest::qWait(50);
        QCOMPARE(done, 0);
        queue.receiveAck(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN, MAV_RESULT_ACCEPTED);
        QTRY_COMPARE_WITH_TIMEOUT(done, 1, 500);
    }
    /** T6: 시간이 넘으면 confirmation을 1씩 올려 재시도하고, 끝내 없으면 Timeout이에요. */
    void test_T6_timeout_retries_with_confirmation()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        int done = 0;
        CommandResult last;
        queue.send(rebootCommand(), {60ms, 2}, [&](CommandResult result) { ++done; last = result; }, this);
        QTRY_COMPARE_WITH_TIMEOUT(done, 1, 2000);
        QCOMPARE(sender.sent.size(), 3);
        QCOMPARE(sender.sent[0].confirmation, std::uint8_t(0));
        QCOMPARE(sender.sent[1].confirmation, std::uint8_t(1));
        QCOMPARE(sender.sent[2].confirmation, std::uint8_t(2));
        QVERIFY(!last.result.has_value());
        QVERIFY(last.failure == CommandFailure::Timeout);
    }
    /** T6: 재시도 뒤에 온 ACK도 그 명령을 끝내요. */
    void test_T6_ack_after_retry()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        int done = 0;
        CommandResult last;
        queue.send(rebootCommand(), {80ms, 3}, [&](CommandResult result) { ++done; last = result; }, this);
        QTRY_COMPARE_WITH_TIMEOUT(sender.sent.size(), 2, 1000);
        queue.receiveAck(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN, MAV_RESULT_ACCEPTED);
        QTRY_COMPARE_WITH_TIMEOUT(done, 1, 500);
        QVERIFY(last.result == MAV_RESULT_ACCEPTED);
        QTest::qWait(200);
        QCOMPARE(sender.sent.size(), 2);
    }
    /** T6: 재시도 0이면 한 번만 보내고 시간이 넘으면 Timeout이에요. */
    void test_T6_zero_retries_sends_once()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        int done = 0;
        CommandResult last;
        queue.send(rebootCommand(), {60ms, 0}, [&](CommandResult result) { ++done; last = result; }, this);
        QTRY_COMPARE_WITH_TIMEOUT(done, 1, 1000);
        QCOMPARE(sender.sent.size(), 1);
        QVERIFY(last.failure == CommandFailure::Timeout);
    }
    /** T6: IN_PROGRESS는 끝이 아니고 시간을 다시 재며 재시도도 하지 않아요. */
    void test_T6_in_progress_extends_timeout()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        int done = 0;
        CommandResult last;
        queue.send(rebootCommand(), {300ms, 1}, [&](CommandResult result) { ++done; last = result; }, this);
        QTRY_COMPARE_WITH_TIMEOUT(sender.sent.size(), 1, 500);
        QTest::qWait(200);
        queue.receiveAck(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN, MAV_RESULT_IN_PROGRESS);
        QTest::qWait(200);
        QCOMPARE(done, 0);
        QCOMPARE(sender.sent.size(), 1);
        queue.receiveAck(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN, MAV_RESULT_ACCEPTED);
        QTRY_COMPARE_WITH_TIMEOUT(done, 1, 500);
        QVERIFY(last.result == MAV_RESULT_ACCEPTED);
    }
    /** T6: 앞 명령이 끝나야 다음 명령이 나가고, 시간 재기도 그때부터예요. */
    void test_T6_next_waits_for_previous()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        QList<int> order;
        int secondDone = 0;
        CommandResult secondResult;
        queue.send(rebootCommand(), {400ms, 0}, [&](CommandResult) { order << 1; }, this);
        queue.send(requestCommand(), {300ms, 0}, [&](CommandResult result) {
            order << 2;
            ++secondDone;
            secondResult = result;
        }, this);
        QTRY_COMPARE_WITH_TIMEOUT(sender.sent.size(), 1, 500);
        QCOMPARE(sender.sent.first().command, std::uint16_t(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN));
        QTest::qWait(350);
        QCOMPARE(sender.sent.size(), 1);
        queue.receiveAck(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN, MAV_RESULT_ACCEPTED);
        QTRY_COMPARE_WITH_TIMEOUT(sender.sent.size(), 2, 500);
        QCOMPARE(sender.sent[1].command, std::uint16_t(MAV_CMD_REQUEST_MESSAGE));
        QCOMPARE(secondDone, 0);
        queue.receiveAck(MAV_CMD_REQUEST_MESSAGE, MAV_RESULT_ACCEPTED);
        QTRY_COMPARE_WITH_TIMEOUT(secondDone, 1, 500);
        QVERIFY(secondResult.result == MAV_RESULT_ACCEPTED);
        QCOMPARE(order, QList<int>({1, 2}));
    }
    /** T6: 앞 명령이 시간 초과로 끝나도 다음 명령은 나가요. */
    void test_T6_next_runs_after_timeout()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        QList<int> order;
        queue.send(rebootCommand(), {50ms, 0}, [&](CommandResult) { order << 1; }, this);
        queue.send(requestCommand(), {50ms, 0}, [&](CommandResult) { order << 2; }, this);
        QTRY_COMPARE_WITH_TIMEOUT(order.size(), 2, 1000);
        QCOMPARE(order, QList<int>({1, 2}));
        QCOMPARE(sender.sent.size(), 2);
    }
    /** T6: stop은 지금 명령과 기다리던 명령을 모두 Gone으로 끝내요. */
    void test_T6_stop_finishes_with_gone()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        QList<CommandResult> results;
        queue.send(rebootCommand(), {}, [&](CommandResult result) { results << result; }, this);
        queue.send(requestCommand(), {}, [&](CommandResult result) { results << result; }, this);
        QTRY_COMPARE_WITH_TIMEOUT(sender.sent.size(), 1, 500);
        queue.stop();
        QTRY_COMPARE_WITH_TIMEOUT(results.size(), 2, 500);
        QVERIFY(results[0].failure == CommandFailure::Gone);
        QVERIFY(results[1].failure == CommandFailure::Gone);
        QCOMPARE(sender.sent.size(), 1);
    }
    /** T6: stop 뒤에 줄 세운 명령도 보내지 않고 Gone으로 끝내요. */
    void test_T6_send_after_stop_is_gone()
    {
        FakeSender sender;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        queue.stop();
        int done = 0;
        CommandResult last;
        queue.send(rebootCommand(), {}, [&](CommandResult result) { ++done; last = result; }, this);
        QTRY_COMPARE_WITH_TIMEOUT(done, 1, 500);
        QVERIFY(last.failure == CommandFailure::Gone);
        QCOMPARE(sender.sent.size(), 0);
    }
    /** T6: 보낼 링크가 없으면 NoLink로 끝내고, 다음 명령으로 넘어가요. */
    void test_T6_no_link()
    {
        FakeSender sender;
        sender.linkUp = false;
        CommandQueue queue(sender.fn(), kVehicleSysid);
        QList<CommandResult> results;
        queue.send(rebootCommand(), {50ms, 0}, [&](CommandResult result) { results << result; }, this);
        queue.send(requestCommand(), {50ms, 0}, [&](CommandResult result) { results << result; }, this);
        QTRY_COMPARE_WITH_TIMEOUT(results.size(), 2, 1000);
        QVERIFY(results[0].failure == CommandFailure::NoLink);
        QVERIFY(results[1].failure == CommandFailure::NoLink);
    }
};
QTEST_GUILESS_MAIN(CommandQueueTest)
#include "test_command_queue.moc"
