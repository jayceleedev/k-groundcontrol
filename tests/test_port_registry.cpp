// [0.2.0 T4] specs/features/0.2.0-fcflash.md
#include <kgc/comm/rawport/PortLease.h>
#include <kgc/comm/rawport/Errors.h>
#include <QtTest>
using kgc::comm::rawport::PortError;
using kgc::comm::rawport::PortLease;
using kgc::comm::rawport::PortRegistry;
using kgc::comm::rawport::make;
using Owner = PortRegistry::Owner;
class PortRegistryTest : public QObject {
    Q_OBJECT
private slots:
    /** T4: 점유하면 주인이 기록되고, 놓으면 다시 비어요. */
    void test_T4_claim_and_release()
    {
        PortRegistry registry;
        QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::None);
        const auto token = registry.claim("/dev/ttyACM0", Owner::Link);
        QVERIFY(token.has_value());
        QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::Link);
        registry.release(*token);
        QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::None);
    }
    /** T4: 다른 주인이 있는 포트의 점유는 Busy이고 기존 주인은 그대로예요. */
    void test_T4_other_owner_is_busy()
    {
        PortRegistry registry;
        const auto link = registry.claim("/dev/ttyACM0", Owner::Link);
        QVERIFY(link.has_value());
        const auto lease = registry.claim("/dev/ttyACM0", Owner::Lease);
        QVERIFY(!lease.has_value());
        QVERIFY(lease.error() == make(PortError::Busy));
        QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::Link);
    }
    /** T4: 포트마다 따로 기록해요. */
    void test_T4_ports_are_independent()
    {
        PortRegistry registry;
        QVERIFY(registry.claim("/dev/ttyACM0", Owner::Link).has_value());
        QVERIFY(registry.claim("/dev/ttyACM1", Owner::Lease).has_value());
        QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::Link);
        QCOMPARE(registry.ownerOf("/dev/ttyACM1"), Owner::Lease);
    }
    /** T4: 이미 놓은 토큰을 다시 놓아도 새 주인을 건드리지 않아요. */
    void test_T4_stale_token_keeps_new_owner()
    {
        PortRegistry registry;
        const auto first = registry.claim("/dev/ttyACM0", Owner::Link);
        QVERIFY(first.has_value());
        registry.release(*first);
        const auto second = registry.claim("/dev/ttyACM0", Owner::Lease);
        QVERIFY(second.has_value());
        registry.release(*first);
        QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::Lease);
    }
    /** T4: 임대가 살아 있는 동안 주인은 Lease이고, 소멸하면 놓여요. */
    void test_T4_lease_releases_on_destruction()
    {
        PortRegistry registry;
        {
            auto lease = PortLease::acquire(registry, "/dev/ttyACM0");
            QVERIFY(lease.has_value());
            QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::Lease);
            const auto again = PortLease::acquire(registry, "/dev/ttyACM0");
            QVERIFY(!again.has_value());
            QVERIFY(again.error() == make(PortError::Busy));
        }
        QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::None);
    }
    /** T4: 링크가 쥔 포트는 임대할 수 없어요. */
    void test_T4_lease_refuses_link_port()
    {
        PortRegistry registry;
        QVERIFY(registry.claim("/dev/ttyACM0", Owner::Link).has_value());
        const auto lease = PortLease::acquire(registry, "/dev/ttyACM0");
        QVERIFY(!lease.has_value());
        QVERIFY(lease.error() == make(PortError::Busy));
    }
    /** T4: 임대는 이동해도 점유가 하나뿐이고, 마지막 주인이 소멸할 때 놓여요. */
    void test_T4_moved_lease_releases_once()
    {
        PortRegistry registry;
        {
            auto first = PortLease::acquire(registry, "/dev/ttyACM0");
            QVERIFY(first.has_value());
            std::optional<PortLease> moved;
            {
                PortLease middle = std::move(*first);
                moved.emplace(std::move(middle));
            }
            QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::Lease);
            moved.reset();
            QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::None);
        }
        QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::None);
    }
    /** T4: 이동 대입은 앞에 쥐고 있던 임대를 놓아요. */
    void test_T4_move_assignment_releases_previous()
    {
        PortRegistry registry;
        auto left = PortLease::acquire(registry, "/dev/ttyACM0");
        auto right = PortLease::acquire(registry, "/dev/ttyACM1");
        QVERIFY(left.has_value() && right.has_value());
        *left = std::move(*right);
        QCOMPARE(registry.ownerOf("/dev/ttyACM0"), Owner::None);
        QCOMPARE(registry.ownerOf("/dev/ttyACM1"), Owner::Lease);
    }
};
QTEST_GUILESS_MAIN(PortRegistryTest)
#include "test_port_registry.moc"
