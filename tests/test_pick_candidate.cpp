// [0.2.0 T12] specs/features/0.2.0-fcflash.md
#include "sources/Candidates.h"
#include <QtTest>
using kgc::fcFlash::CandidatePick;
using kgc::fcFlash::pickCandidate;
class PickCandidateTest : public QObject {
    Q_OBJECT
private slots:
    /** T12: 후보가 없으면 None이에요. */
    void test_T12_none()
    {
        QCOMPARE(pickCandidate(0), CandidatePick::None);
    }
    /** T12: 후보가 하나면 Auto예요. */
    void test_T12_auto()
    {
        QCOMPARE(pickCandidate(1), CandidatePick::Auto);
    }
    /** T12: 후보가 여럿이면 Choose예요. */
    void test_T12_choose()
    {
        QCOMPARE(pickCandidate(2), CandidatePick::Choose);
        QCOMPARE(pickCandidate(14), CandidatePick::Choose);
    }
};
QTEST_APPLESS_MAIN(PickCandidateTest)
#include "test_pick_candidate.moc"
