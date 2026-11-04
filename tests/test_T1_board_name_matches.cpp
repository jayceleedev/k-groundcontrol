// [0.4.1 T1] specs/features/0.4.1-board-match.md
#include "sources/BoardMatch.h"
#include <QtTest>
using kgc::fcFlash::nameMatches;
class BoardNameMatchesTest : public QObject {
    Q_OBJECT
private slots:
    /** T1: 대소문자와 앞뒤 공백은 무시하고 같은 이름이면 참이에요. */
    void test_T1_ignores_case_and_outer_spaces()
    {
        QVERIFY(nameMatches("MatekH743", {"MatekH743"}));
        QVERIFY(nameMatches("  matekh743\n", {"MATEKH743"}));
        QVERIFY(nameMatches("KoaFC", {"Other", " koafc "}));
    }
    /** T1: USB 이름이 비었거나 공백뿐이면 거짓이에요. */
    void test_T1_empty_name_is_false()
    {
        QVERIFY(!nameMatches("", {"MatekH743", ""}));
        QVERIFY(!nameMatches("   ", {"MatekH743", "   "}));
    }
    /** T1: 부분 일치는 거짓이에요. -BL 같은 꼬리도 떼지 않아요. */
    void test_T1_partial_match_is_false()
    {
        QVERIFY(!nameMatches("MatekH743", {"MatekH743-bdshot"}));
        QVERIFY(!nameMatches("MatekH743-bdshot", {"MatekH743"}));
        QVERIFY(!nameMatches("MatekH743-BL", {"MatekH743"}));
        QVERIFY(nameMatches("MatekH743-BL", {"MatekH743", "MatekH743-BL"}));
    }
    /** T1: 맞춰 볼 이름이 없으면 거짓이에요. */
    void test_T1_no_names_is_false()
    {
        QVERIFY(!nameMatches("MatekH743", {}));
    }
};
QTEST_APPLESS_MAIN(BoardNameMatchesTest)
#include "test_T1_board_name_matches.moc"
