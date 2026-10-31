// [0.2.0 T4] specs/features/0.2.0-fcflash.md
#include <kgc/comm/rawport/PortLease.h>
namespace kgc::comm::rawport {
kgc::app::errors::Result<PortLease> PortLease::acquire(PortRegistry& registry, QString portName)
{
    auto token = registry.claim(std::move(portName), PortRegistry::Owner::Lease);
    if (!token) return std::unexpected(token.error());
    return PortLease(registry, *token);
}
PortLease::PortLease(PortRegistry& registry, PortRegistry::Token token) : registry_(&registry), token_(token) {}
PortLease::PortLease(PortLease&& other) noexcept
{
    // WORKER-FILL: spec §5.5 — other의 점유를 넘겨받고 other는 아무것도 쥐지 않게 해요.
    Q_UNUSED(other);
}
PortLease& PortLease::operator=(PortLease&& other) noexcept
{
    // WORKER-FILL: spec §5.5 — 지금 쥔 점유를 놓고 other의 점유를 넘겨받아요.
    Q_UNUSED(other);
    return *this;
}
PortLease::~PortLease()
{
    // WORKER-FILL: spec §5.5 — 쥔 점유가 있으면 등록부에 release해요.
}
}
