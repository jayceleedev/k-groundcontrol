// [0.0.2 T20] specs/features/0.0.2-theme.md
#include <kgc/app/settings/Range.h>
#include <QtTest>
#include <limits>
using kgc::app::settings::withinRange;
/** 설정 경계를 포함하고 범위 밖 값을 거부하는지 확인해요.
 * 입력: 숫자 설정 정의와 시험 값.
 * 출력: 시험 결과.
 * 실패: 범위 판정이 다르면 실패해요.
 */
class SettingsRangeTest : public QObject {
    Q_OBJECT
private slots:
    /** T20: 정수의 양 끝은 허용하고 경계 밖은 거부해요. */
    void test_T20_integer_boundaries()
    {
        const QJsonObject definition{{"type", "int"}, {"min", 500}, {"max", 5000}};
        QVERIFY(withinRange(definition, 500));
        QVERIFY(withinRange(definition, 5000));
        QVERIFY(withinRange(definition, 1500));
        QVERIFY(!withinRange(definition, 499));
        QVERIFY(!withinRange(definition, 5001));
        QVERIFY(!withinRange(definition, "invalid"));
    }
    /** T20: 소수와 한쪽만 있는 경계도 같은 규칙으로 확인해요. */
    void test_T20_double_and_optional_bounds()
    {
        const QJsonObject definition{{"type", "double"}, {"min", -1.5}, {"max", 2.5}};
        QVERIFY(withinRange(definition, -1.5));
        QVERIFY(withinRange(definition, 2.5));
        QVERIFY(!withinRange(definition, -1.6));
        QVERIFY(!withinRange(definition, 2.6));
        QVERIFY(!withinRange(definition, std::numeric_limits<double>::quiet_NaN()));
        QVERIFY(!withinRange(definition, std::numeric_limits<double>::infinity()));
        QVERIFY(withinRange({{"type", "int"}, {"min", 2}}, 9));
        QVERIFY(!withinRange({{"type", "int"}, {"min", 2}}, 1));
        QVERIFY(withinRange({{"type", "double"}, {"max", 2.5}}, -9.0));
        QVERIFY(!withinRange({{"type", "double"}, {"max", 2.5}}, 3.0));
        QVERIFY(withinRange({{"type", "string"}}, "text"));
        QVERIFY(withinRange({{"type", "int"}}, 9));
    }
};
QTEST_GUILESS_MAIN(SettingsRangeTest)
#include "test_settings_range.moc"
