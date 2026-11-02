// [0.3.0 T5] specs/features/0.3.0-params.md
#include "Constants.h"
#include "ParamDownload.h"
#include <QtTest>
using namespace kgc::params;
class ParamGapTest : public QObject {
    Q_OBJECT
private slots:
    /** T5: 한 번에 10개까지, 번호 0부터 안 받은 것만 골라요. */
    void test_T5_batch_limit()
    {
        QHash<int, int> tries;
        const auto requests = nextRequests(30, {0, 2}, tries);
        QCOMPARE(requests.size(), kParamReadBatch);
        QCOMPARE(requests.first(), 1);
        QCOMPARE(requests[1], 3);
        QCOMPARE(tries.value(1), 1);
        QVERIFY(!tries.contains(0));
    }
    /** T5: 번호마다 5회까지만 고르고, 다 쓰면 빈 목록이에요. */
    void test_T5_try_limit()
    {
        QHash<int, int> tries;
        for (int i = 0; i < kParamReadTries; ++i) QCOMPARE(nextRequests(2, {0}, tries), QList<int>{1});
        QCOMPARE(tries.value(1), kParamReadTries);
        QVERIFY(nextRequests(2, {0}, tries).isEmpty());
    }
    /** T5: 전부 받았으면 고를 것이 없어요. */
    void test_T5_all_received()
    {
        QHash<int, int> tries;
        QVERIFY(nextRequests(2, {0, 1}, tries).isEmpty());
    }
};
QTEST_APPLESS_MAIN(ParamGapTest)
#include "test_T5_param_gap.moc"
