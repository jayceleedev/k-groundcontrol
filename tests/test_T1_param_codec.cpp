// [0.3.0 T1] specs/features/0.3.0-params.md
#include "ParamCodec.h"
#include <kgc/comm/mavlink/Mavlink.h>
#include <QtTest>
#include <bit>
using kgc::params::decodeParam;
using kgc::params::encodeParam;
using kgc::vehicle::firmware::ParamEncoding;
class ParamCodecTest : public QObject {
    Q_OBJECT
private slots:
    /** T1: Bytewise 정수는 float 비트에 그대로 담겨서 큰 값과 음수도 왕복해요. */
    void test_T1_bytewise_integers()
    {
        const auto big = encodeParam(16777217, MAV_PARAM_TYPE_INT32, ParamEncoding::Bytewise);
        QCOMPARE(std::bit_cast<std::int32_t>(big), 16777217);
        QCOMPARE(decodeParam(big, MAV_PARAM_TYPE_INT32, ParamEncoding::Bytewise), 16777217.0);
        const auto negative = encodeParam(-1, MAV_PARAM_TYPE_INT32, ParamEncoding::Bytewise);
        QCOMPARE(std::bit_cast<std::uint32_t>(negative), 0xFFFFFFFFU);
        QCOMPARE(decodeParam(negative, MAV_PARAM_TYPE_INT32, ParamEncoding::Bytewise), -1.0);
    }
    /** T1: Bytewise 실수는 float 그대로예요. */
    void test_T1_bytewise_real()
    {
        const auto wire = encodeParam(0.1, MAV_PARAM_TYPE_REAL32, ParamEncoding::Bytewise);
        QCOMPARE(wire, 0.1F);
        QCOMPARE(static_cast<float>(decodeParam(wire, MAV_PARAM_TYPE_REAL32, ParamEncoding::Bytewise)), 0.1F);
    }
    /** T1: CCast 정수는 float 숫자로 그대로 옮겨서 왕복해요. */
    void test_T1_ccast_integers()
    {
        QCOMPARE(encodeParam(-5, MAV_PARAM_TYPE_INT8, ParamEncoding::CCast), -5.0F);
        QCOMPARE(decodeParam(-5.0F, MAV_PARAM_TYPE_INT8, ParamEncoding::CCast), -5.0);
        QCOMPARE(decodeParam(encodeParam(300, MAV_PARAM_TYPE_INT16, ParamEncoding::CCast), MAV_PARAM_TYPE_INT16, ParamEncoding::CCast), 300.0);
        QCOMPARE(decodeParam(encodeParam(1000000, MAV_PARAM_TYPE_INT32, ParamEncoding::CCast), MAV_PARAM_TYPE_INT32, ParamEncoding::CCast), 1000000.0);
    }
    /** T1: Bytewise의 짧은 정수는 낮은 바이트에만 담겨요. */
    void test_T1_bytewise_short_integers()
    {
        const auto wire = encodeParam(-5, MAV_PARAM_TYPE_INT8, ParamEncoding::Bytewise);
        QCOMPARE(std::bit_cast<std::uint32_t>(wire), 0xFBU);
        QCOMPARE(decodeParam(wire, MAV_PARAM_TYPE_INT8, ParamEncoding::Bytewise), -5.0);
    }
};
QTEST_APPLESS_MAIN(ParamCodecTest)
#include "test_T1_param_codec.moc"
