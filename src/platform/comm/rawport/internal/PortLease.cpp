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
    registry_ = other.registry_;
    token_ = other.token_;
    other.registry_.clear();
    other.token_ = {};
}
PortLease& PortLease::operator=(PortLease&& other) noexcept
{
    if (this == &other) return *this;
    if (registry_) registry_->release(token_);
    registry_ = other.registry_;
    token_ = other.token_;
    other.registry_.clear();
    other.token_ = {};
    return *this;
}
PortLease::~PortLease()
{
    if (registry_) registry_->release(token_);
}
}
