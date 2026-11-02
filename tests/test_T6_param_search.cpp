// [0.3.0 T6] specs/features/0.3.0-params.md
#include "ParamSearch.h"
#include <QtTest>
#include <algorithm>
using namespace kgc::params;
class ParamSearchTest : public QObject {
    Q_OBJECT
private slots:
    /** T6: 숫자 구간은 수로 견줘서 RC2_가 RC10_ 앞이에요. */
    void test_T6_natural_order()
    {
        QVERIFY(naturalLess("RC2_MIN", "RC10_MIN"));
        QVERIFY(!naturalLess("RC10_MIN", "RC2_MIN"));
        QVERIFY(!naturalLess("RC2_MIN", "RC2_MIN"));
        QVERIFY(naturalLess("RC2", "RC2_MIN"));
        QStringList names{"SERVO10", "ATC", "SERVO2", "SERVO1"};
        std::sort(names.begin(), names.end(), naturalLess);
        QCOMPARE(names, QStringList({"ATC", "SERVO1", "SERVO2", "SERVO10"}));
    }
    /** T6: 앞의 0만 다른 이름도 서로 다른 순서를 가져요. */
    void test_T6_leading_zero_is_ordered()
    {
        QVERIFY(naturalLess("A01", "A1") != naturalLess("A1", "A01"));
    }
    /** T6: 공백으로 나눈 낱말 전부가 셋 중 하나에 대소문자 없이 들어 있어야 해요. */
    void test_T6_and_terms()
    {
        const auto terms = searchTerms("  batt   CAPACITY ");
        QCOMPARE(terms, QStringList({"batt", "CAPACITY"}));
        QVERIFY(matchesSearch(terms, "BATT_CAPACITY", "", ""));
        QVERIFY(matchesSearch(terms, "BATT_X", "Pack capacity", ""));
        QVERIFY(matchesSearch(terms, "X", "battery", "Total capacity"));
        QVERIFY(!matchesSearch(terms, "BATT_X", "", ""));
        QVERIFY(matchesSearch({}, "ANY", "", ""));
    }
};
QTEST_APPLESS_MAIN(ParamSearchTest)
#include "test_T6_param_search.moc"
