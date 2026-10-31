// [0.1.0 T3] specs/features/0.1.0-link.md
#include <kgc/vehicle/states/LinkLoss.h>
#include <QtTest>
class LinkLossTest : public QObject {
    Q_OBJECT
private slots:
    /** T3: 마지막 하트비트에서 정확히 3500밀리초일 때 끊김으로 판정해요. */
    void test_T3_threshold()
    {
        using kgc::vehicle::states::isLinkLost;
        QVERIFY(!isLinkLost(10000, 13499));
        QVERIFY(isLinkLost(10000, 13500));
        QVERIFY(isLinkLost(10000, 15000));
        QVERIFY(!isLinkLost(-1, 10000));
        QVERIFY(!isLinkLost(10000, 9999));
    }
};
QTEST_GUILESS_MAIN(LinkLossTest)
#include "test_link_loss.moc"
