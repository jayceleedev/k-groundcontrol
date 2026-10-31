// [0.1.0 T1] specs/features/0.1.0-link.md
#include <kgc/comm/mavlink/MavlinkParser.h>
#include <QtTest>
using namespace kgc::comm::mavlink;
class MavlinkParserTest : public QObject {
    Q_OBJECT
private slots:
    /** T1: 두 버전의 하트비트와 자세 시험 벡터를 준비해요. */
    void test_T1_vectors_data()
    {
        QTest::addColumn<QByteArray>("frame");
        QTest::addColumn<bool>("v1");
        QTest::addColumn<int>("id");
        QTest::newRow("v1-heartbeat") << QByteArray::fromHex("fe090701010000000000020c800403c523") << true << 0;
        QTest::newRow("v1-attitude") << QByteArray::fromHex("fe1c0701011ed2040000cdcccc3dcdcc4cbe000000bf00000000000000000000000038a8") << true << 30;
        QTest::newRow("v2-heartbeat") << QByteArray::fromHex("fd09000007010100000000000000020c8004039b22") << false << 0;
        QTest::newRow("v2-attitude") << QByteArray::fromHex("fd1c00000701011e0000d2040000cdcccc3dcdcc4cbe000000bf0000000000000000000000002fdf") << false << 30;
    }
    /** T1: 독립적으로 만든 고정 바이트가 버전과 메시지 값을 보존해요. */
    void test_T1_vectors()
    {
        QFETCH(QByteArray, frame);
        QFETCH(bool, v1);
        QFETCH(int, id);
        MavlinkParser parser;
        mavlink_message_t message{};
        bool isV1 = false;
        int complete = 0;
        for (auto byte : frame) if (parser.parse(static_cast<std::uint8_t>(byte), message, isV1)) ++complete;
        QCOMPARE(complete, 1);
        QCOMPARE(isV1, v1);
        QCOMPARE(int(message.msgid), id);
        QCOMPARE(message.sysid, 1u);
        QCOMPARE(message.compid, 1);
        if (id == MAVLINK_MSG_ID_ATTITUDE) {
            mavlink_attitude_t attitude{};
            mavlink_msg_attitude_decode(&message, &attitude);
            QCOMPARE(attitude.time_boot_ms, 1234u);
            QCOMPARE(attitude.yaw, -0.5f);
        } else {
            mavlink_heartbeat_t heartbeat{};
            mavlink_msg_heartbeat_decode(&message, &heartbeat);
            QCOMPARE(heartbeat.autopilot, 12);
            QCOMPARE(heartbeat.base_mode, 128);
        }
        QCOMPARE(dropV1NonHeartbeat(isV1, message.msgid), v1 && id != 0);
    }
    /** T1: 잘린 프레임을 기다리고 이어 붙인 프레임을 각각 완성해요. */
    void test_T1_fragmented_and_concatenated()
    {
        const auto frame = QByteArray::fromHex("fd09000007010100000000000000020c8004039b22");
        MavlinkParser parser;
        mavlink_message_t message{};
        bool v1 = false;
        for (const auto byte : frame.first(frame.size() - 1)) QVERIFY(!parser.parse(byte, message, v1));
        QVERIFY(parser.parse(frame.back(), message, v1));
        int complete = 0;
        for (const auto byte : frame + frame) if (parser.parse(byte, message, v1)) ++complete;
        QCOMPARE(complete, 2);
    }
    /** T1: 두 버전에서 마지막 바이트가 유실된 프레임을 준비해요. */
    void test_T1_abandoned_truncated_frame_data()
    {
        QTest::addColumn<QByteArray>("frame");
        QTest::addColumn<bool>("expectedV1");
        QTest::newRow("v1") << QByteArray::fromHex("fe090701010000000000020c800403c523") << true;
        QTest::newRow("v2") << QByteArray::fromHex("fd09000007010100000000000000020c8004039b22") << false;
    }
    /** T1: 끝의 CRC 바이트가 유실되어도 다음 정상 프레임을 복구해요. */
    void test_T1_abandoned_truncated_frame()
    {
        QFETCH(QByteArray, frame);
        QFETCH(bool, expectedV1);
        MavlinkParser parser;
        mavlink_message_t message{};
        bool v1 = true;
        for (const auto byte : frame.first(frame.size() - 1)) QVERIFY(!parser.parse(byte, message, v1));
        int complete = 0;
        for (const auto byte : frame) if (parser.parse(byte, message, v1)) ++complete;
        QCOMPARE(complete, 1);
        QCOMPARE(message.msgid, MAVLINK_MSG_ID_HEARTBEAT);
        QCOMPARE(message.sysid, 1);
        QCOMPARE(v1, expectedV1);
    }
    /** T1: CRC 오류 뒤의 정상 프레임을 복구하고 링크별 파싱 상태를 분리해요. */
    void test_T1_crc_and_independent_parsers()
    {
        const auto frame = QByteArray::fromHex("fd09000007010100000000000000020c8004039b22");
        auto corrupt = frame;
        corrupt[corrupt.size() - 1] ^= 1;
        MavlinkParser first, second;
        mavlink_message_t message{};
        bool v1 = false;
        for (const auto byte : corrupt) QVERIFY(!first.parse(byte, message, v1));
        int count = 0;
        for (const auto byte : frame) {
            if (first.parse(byte, message, v1)) ++count;
            if (second.parse(byte, message, v1)) ++count;
        }
        QCOMPARE(count, 2);
    }
};
QTEST_GUILESS_MAIN(MavlinkParserTest)
#include "test_mavlink_parser.moc"
