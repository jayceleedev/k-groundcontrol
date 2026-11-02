// [0.3.0 T3] specs/features/0.3.0-params.md
#include "ParamFile.h"
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/params/Errors.h>
#include <QtTest>
using namespace kgc::params;
class ParamFileTest : public QObject {
    Q_OBJECT
private slots:
    /** T3: QGC 다섯 칸 줄에서 이름과 값을 읽고 다른 컴포넌트 줄은 세기만 해요. */
    void test_T3_qgc_columns()
    {
        const auto parsed = parseParamFile("# 주석\n\n1\t1\tBAT1_N_CELLS\t4\t6\n1\t2\tGIMBAL_X\t7\t6\n1\t1\tBAT1_V_CHARGED\t4.05\t9\n");
        QVERIFY(parsed.has_value());
        QCOMPARE(parsed->entries.size(), 2);
        QCOMPARE(parsed->entries[0].first, QString("BAT1_N_CELLS"));
        QCOMPARE(parsed->entries[1].second, 4.05);
        QCOMPARE(parsed->otherComponent, 1);
        QCOMPARE(parsed->badLines, 0);
    }
    /** T3: Mission Planner 두 칸은 쉼표와 공백 둘 다 읽고 셋째 칸은 버려요. */
    void test_T3_mp_columns()
    {
        const auto parsed = parseParamFile("ACRO_BAL,1.5\r\nATC_RAT 0.2 extra\n");
        QVERIFY(parsed.has_value());
        QCOMPARE(parsed->entries.size(), 2);
        QCOMPARE(parsed->entries[0].second, 1.5);
        QCOMPARE(parsed->entries[1].first, QString("ATC_RAT"));
        QCOMPARE(parsed->entries[1].second, 0.2);
    }
    /** T3: 값이 숫자가 아니거나 이름이 비면 나쁜 줄로 세요. */
    void test_T3_bad_lines()
    {
        const auto parsed = parseParamFile("GOOD,1\nBAD,abc\n,5\nALONE\n1\t1\t\t3\t6\n");
        QVERIFY(parsed.has_value());
        QCOMPARE(parsed->entries.size(), 1);
        QCOMPARE(parsed->badLines, 4);
    }
    /** T3: 같은 이름이 또 나오면 뒤 값을 써요. */
    void test_T3_duplicate_takes_last()
    {
        const auto parsed = parseParamFile("A,1\nB,2\nA,3\n");
        QVERIFY(parsed.has_value());
        QCOMPARE(parsed->entries.size(), 2);
        QCOMPARE(parsed->entries[0].second, 3.0);
    }
    /** T3: 읽은 값이 없으면 FileEmpty예요. */
    void test_T3_empty()
    {
        QCOMPARE(parseParamFile("# 주석만\n\n").error(), make(ParamError::FileEmpty));
    }
    /** T3: 쓴 파일을 다시 읽으면 값이 같고, 대기값은 들어가지 않으며 이름은 자연순이에요. */
    void test_T3_round_trip()
    {
        const QList<Param> rows{{"RC10_MIN", 1000, MAV_PARAM_TYPE_INT16, 5.0, {}},
                                {"RC2_MIN", 1100, MAV_PARAM_TYPE_INT16, std::nullopt, {}},
                                {"BAT1_V_CHARGED", 4.05F, MAV_PARAM_TYPE_REAL32, std::nullopt, {}}};
        const auto bytes = writeParamFile({7, "PX4", 2, "1.15.2"}, rows);
        QVERIFY(bytes.startsWith("# Onboard parameters for Vehicle 7\n#\n# Stack: PX4\n# Vehicle: 2\n# Version: 1.15.2\n#\n"
                                 "# Vehicle-Id\tComponent-Id\tName\tValue\tType\n7\t1\tBAT1_V_CHARGED\t4.05\t9\n"));
        const auto parsed = parseParamFile(bytes);
        QVERIFY(parsed.has_value());
        QCOMPARE(parsed->entries.size(), 3);
        QCOMPARE(parsed->entries[1].first, QString("RC2_MIN"));
        QCOMPARE(parsed->entries[2].first, QString("RC10_MIN"));
        QCOMPARE(parsed->entries[2].second, 1000.0);
    }
    /** T2: 파일에도 큰 실수가 지수 표기 없이 들어가요. */
    void test_T2_file_uses_fixed_notation()
    {
        const auto bytes = writeParamFile({1, "ArduPilot", 2, "4.7.2"}, {{"ATC_ACCEL_MAX", 100000, MAV_PARAM_TYPE_REAL32, std::nullopt, {}}});
        QVERIFY(bytes.endsWith("1\t1\tATC_ACCEL_MAX\t100000\t9\n"));
    }
};
QTEST_APPLESS_MAIN(ParamFileTest)
#include "test_T3_param_file.moc"
