// [0.1.1 T4] specs/features/0.1.1-link-hardening.md
#include "UdpPeers.h"
#include <QtTest>
using kgc::comm::links::UdpPeers;
class UdpPeersTest : public QObject {
    Q_OBJECT
private slots:
    /** T4: 주소와 포트가 같으면 하나로 세고, 다르면 따로 세요. */
    void test_T4_counts_each_address_once()
    {
        UdpPeers peers;
        const QHostAddress local(QHostAddress::LocalHost);
        peers.note(local, 1000, 0);
        peers.note(local, 1000, 5);
        QCOMPARE(peers.active(10).size(), 1);
        peers.note(local, 2000, 10);
        const auto active = peers.active(10);
        QCOMPARE(active.size(), 2);
        QCOMPARE(active[0].second, quint16(1000));
        QCOMPARE(active[1].second, quint16(2000));
    }
    /** T4: 10초가 지난 주소는 빠지고, 다시 받으면 목록 끝에 돌아와요. */
    void test_T4_expires_after_ten_seconds()
    {
        UdpPeers peers;
        const QHostAddress local(QHostAddress::LocalHost);
        peers.note(local, 1000, 0);
        peers.note(local, 2000, 1000);
        QCOMPARE(peers.active(9999).size(), 2);
        auto active = peers.active(10000);
        QCOMPARE(active.size(), 1);
        QCOMPARE(active[0].second, quint16(2000));
        peers.note(local, 1000, 10500);
        active = peers.active(10999);
        QCOMPARE(active.size(), 2);
        QCOMPARE(active[0].second, quint16(2000));
        QCOMPARE(active[1].second, quint16(1000));
        active = peers.active(11000);
        QCOMPARE(active.size(), 1);
        QCOMPARE(active[0].second, quint16(1000));
    }
};
QTEST_GUILESS_MAIN(UdpPeersTest)
#include "test_udp_peers.moc"
