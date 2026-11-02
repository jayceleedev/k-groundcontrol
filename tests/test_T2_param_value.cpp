// [0.3.0 T2] specs/features/0.3.0-params.md
#include "ParamValue.h"
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/params/Errors.h>
#include <QtTest>
#include <cmath>
#include <limits>
using namespace kgc::params;
class ParamValueTest : public QObject {
    Q_OBJECT
private slots:
    /** T2: 실수는 float가 왕복하는 가장 짧은 표기로, 정수는 정수로 보여요. */
    void test_T2_format()
    {
        QCOMPARE(formatValue(0.1F, MAV_PARAM_TYPE_REAL32), QString("0.1"));
        QCOMPARE(formatValue(4.05, MAV_PARAM_TYPE_REAL32), QString("4.05"));
        QCOMPARE(formatValue(3, MAV_PARAM_TYPE_INT32), QString("3"));
    }
    /** T2: 실수는 지수 표기 없이 고정 소수점으로 보여서 100000은 그대로 100000이에요. */
    void test_T2_format_fixed()
    {
        QCOMPARE(formatValue(100000, MAV_PARAM_TYPE_REAL32), QString("100000"));
        QCOMPARE(formatValue(1000000, MAV_PARAM_TYPE_REAL32), QString("1000000"));
        QCOMPARE(formatValue(0.00001F, MAV_PARAM_TYPE_REAL32), QString("0.00001"));
        QCOMPARE(formatValue(100000, MAV_PARAM_TYPE_REAL64), QString("100000"));
        // 형의 끝 값도 잘리지 않고 원래 값으로 돌아와야 해요.
        const auto largest = std::numeric_limits<float>::max();
        QCOMPARE(static_cast<float>(formatValue(largest, MAV_PARAM_TYPE_REAL32).toDouble()), largest);
        const auto smallest = std::numeric_limits<float>::denorm_min();
        QCOMPARE(static_cast<float>(formatValue(smallest, MAV_PARAM_TYPE_REAL32).toDouble()), smallest);
        const auto widest = std::numeric_limits<double>::max();
        QCOMPARE(formatValue(widest, MAV_PARAM_TYPE_REAL64).toDouble(), widest);
        const auto tiniest = std::numeric_limits<double>::denorm_min();
        QCOMPARE(formatValue(-tiniest, MAV_PARAM_TYPE_REAL64).toDouble(), -tiniest);
        ParamMeta meta;
        meta.min = -1;
        meta.max = 100000;
        QCOMPARE(formatRange(meta, MAV_PARAM_TYPE_REAL32), QString("-1 ~ 100000"));
    }
    /** T2: 쉼표는 소수점으로 읽고, 나쁜 입력은 까닭별 오류예요. */
    void test_T2_parse()
    {
        QCOMPARE(parseValue(" 1,5 ", MAV_PARAM_TYPE_REAL32).value(), 1.5);
        QCOMPARE(parseValue("abc", MAV_PARAM_TYPE_REAL32).error(), make(ParamError::NotANumber));
        QCOMPARE(parseValue("inf", MAV_PARAM_TYPE_REAL32).error(), make(ParamError::NotANumber));
        QCOMPARE(parseValue("300", MAV_PARAM_TYPE_INT8).error(), make(ParamError::TypeLimit));
        QCOMPARE(parseValue("1.5", MAV_PARAM_TYPE_INT32).error(), make(ParamError::NotInteger));
    }
    /** T2: float로 같으면 같은 값이고, 정수형은 정확히 같아야 해요. */
    void test_T2_same()
    {
        QVERIFY(sameValue(4.1500001, 4.15, MAV_PARAM_TYPE_REAL32));
        QVERIFY(!sameValue(4.15, 4.2, MAV_PARAM_TYPE_REAL32));
        QVERIFY(!sameValue(1, 2, MAV_PARAM_TYPE_INT32));
        QVERIFY(!sameValue(1013.25, 1013.26, MAV_PARAM_TYPE_REAL32));
    }
    /** T2: float 2 ULP까지만 같고, 0 둘레의 부호가 달라도 거리로 재요. */
    void test_T2_same_ulps()
    {
        const auto base = 1013.25F;
        const auto one = std::nextafter(base, 2000.0F);
        const auto two = std::nextafter(one, 2000.0F);
        const auto three = std::nextafter(two, 2000.0F);
        QVERIFY(sameValue(base, one, MAV_PARAM_TYPE_REAL32));
        QVERIFY(sameValue(base, two, MAV_PARAM_TYPE_REAL32));
        QVERIFY(!sameValue(base, three, MAV_PARAM_TYPE_REAL32));
        QVERIFY(sameValue(0.0, -0.0, MAV_PARAM_TYPE_REAL32));
        const auto tiny = std::numeric_limits<float>::denorm_min();
        QVERIFY(sameValue(tiny, -tiny, MAV_PARAM_TYPE_REAL32));
        const auto nan = std::numeric_limits<float>::quiet_NaN();
        QVERIFY(!sameValue(nan, 1.0, MAV_PARAM_TYPE_REAL32));
    }
    /** T2: 비트 계산은 값의 형 폭 안에서 하고, 목록에 없는 비트는 그대로 둬요. */
    void test_T2_bits()
    {
        QVERIFY(bitIsSet(-2147483648.0, MAV_PARAM_TYPE_INT32, 31));
        QVERIFY(!bitIsSet(-2147483648.0, MAV_PARAM_TYPE_INT32, 30));
        QVERIFY(bitIsSet(-1, MAV_PARAM_TYPE_INT32, 0));
        QVERIFY(!bitIsSet(255, MAV_PARAM_TYPE_UINT8, 8));
        QCOMPARE(withBits(0b1100, MAV_PARAM_TYPE_INT32, {0, 1}, {0}), 13.0);
        QCOMPARE(withBits(0, MAV_PARAM_TYPE_INT32, {31}, {31}), -2147483648.0);
        QCOMPARE(withBits(-2147483648.0, MAV_PARAM_TYPE_INT32, {31}, {}), 0.0);
        QCOMPARE(withBits(-1, MAV_PARAM_TYPE_INT32, {31}, {}), 2147483647.0);
        QCOMPARE(withBits(0, MAV_PARAM_TYPE_UINT32, {31}, {31}), 2147483648.0);
        QCOMPARE(withBits(-1, MAV_PARAM_TYPE_INT8, {7}, {}), 127.0);
        QCOMPARE(withBits(1, MAV_PARAM_TYPE_UINT8, {8}, {8}), 1.0);
    }
    /** T2: 메타데이터가 없으면 범위 밖이 아니고, 범위는 양쪽 다 봐요. */
    void test_T2_range()
    {
        ParamMeta meta;
        meta.min = 0;
        meta.max = 5;
        QVERIFY(!outOfRange(9, nullptr));
        QVERIFY(!outOfRange(5, &meta));
        QVERIFY(outOfRange(-1, &meta));
        QVERIFY(outOfRange(6, &meta));
        QCOMPARE(formatRange(meta, MAV_PARAM_TYPE_REAL32), QString("0 ~ 5"));
    }
};
QTEST_APPLESS_MAIN(ParamValueTest)
#include "test_T2_param_value.moc"
