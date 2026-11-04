// [0.4.1 T2] specs/features/0.4.1-board-match.md
#include "sources/BoardMatch.h"
#include <QtTest>
using kgc::fcFlash::BoardMatch;
using kgc::fcFlash::MatchResult;
using kgc::fcFlash::preselect;
using kgc::fcFlash::recognize;
namespace {
/** ArduPilot 보드 ID 1013 Copter 안정판의 두 후보와 그 이름들을 흉내 내요. */
QList<QStringList> matekCandidates()
{
    return {{"MatekH743", "MatekH743-BL"}, {"MatekH743-bdshot", "MatekH743-BL"}};
}
}
class BoardPreselectTest : public QObject {
    Q_OBJECT
private slots:
    /** T2: USB 이름이 비면 NoName이고, 0.2.0처럼 후보가 하나일 때만 0을 골라 둬요. */
    void test_T2_no_name_follows_0_2_0()
    {
        const auto none = recognize("", {});
        QCOMPARE(none.kind, BoardMatch::NoName);
        QVERIFY(none.matched.isEmpty());
        QCOMPARE(preselect(none, 0), -1);
        const auto one = recognize(" ", {{"MatekH743"}});
        QCOMPARE(one.kind, BoardMatch::NoName);
        QCOMPARE(preselect(one, 1), 0);
        const auto many = recognize("", matekCandidates());
        QCOMPARE(many.kind, BoardMatch::NoName);
        QVERIFY(many.matched.isEmpty());
        QCOMPARE(preselect(many, 2), -1);
    }
    /** T2: 이름이 후보 하나와만 맞으면 Matched이고 그 위치를 골라 둬요. */
    void test_T2_one_match_picks_it()
    {
        const auto result = recognize("matekh743-bdshot", {{"MatekH743"}, {"MatekH743-bdshot"}, {"Other"}});
        QCOMPARE(result.kind, BoardMatch::Matched);
        QCOMPARE(result.matched, QList<int>({1}));
        QCOMPARE(preselect(result, 3), 1);
    }
    /** T2: 이름이 여러 후보와 맞으면 Matched이고 맞은 위치를 순서대로 내지만 골라 두지 않아요. */
    void test_T2_many_matches_pick_nothing()
    {
        const auto result = recognize("MatekH743-BL", matekCandidates());
        QCOMPARE(result.kind, BoardMatch::Matched);
        QCOMPARE(result.matched, QList<int>({0, 1}));
        QCOMPARE(preselect(result, 2), -1);
    }
    /** T2: 이름이 있는데 아무 후보와도 안 맞으면 NoMatch이고 골라 두지 않아요. 후보가 하나여도 마찬가지예요. */
    void test_T2_no_match_picks_nothing()
    {
        const auto result = recognize("KoaFC", matekCandidates());
        QCOMPARE(result.kind, BoardMatch::NoMatch);
        QVERIFY(result.matched.isEmpty());
        QCOMPARE(preselect(result, 2), -1);
        const auto single = recognize("KoaFC", {{"MatekH743"}});
        QCOMPARE(single.kind, BoardMatch::NoMatch);
        QCOMPARE(preselect(single, 1), -1);
        QCOMPARE(recognize("KoaFC", {}).kind, BoardMatch::NoMatch);
    }
};
QTEST_APPLESS_MAIN(BoardPreselectTest)
#include "test_T2_board_preselect.moc"
