// [0.2.0 T4] specs/features/0.2.0-fcflash.md
#include <kgc/comm/rawport/PortRegistry.h>
#include <kgc/comm/rawport/Errors.h>
namespace kgc::comm::rawport {
struct PortRegistry::Private {};
PortRegistry::PortRegistry(QObject* parent) : QObject(parent), d_(std::make_unique<Private>()) {}
PortRegistry::~PortRegistry() = default;
kgc::app::errors::Result<PortRegistry::Token> PortRegistry::claim(QString portName, Owner owner)
{
    // WORKER-FILL: spec §5.5 — 비어 있으면 주인을 적고 Token을 돌려줘요. 주인이 있으면 PortError::Busy예요.
    Q_UNUSED(portName);
    Q_UNUSED(owner);
    return std::unexpected(make(PortError::Busy));
}
void PortRegistry::release(Token token)
{
    // WORKER-FILL: spec §5.5 — 토큰의 포트 주인을 지워요. 모르는 토큰은 무시해요.
    Q_UNUSED(token);
}
PortRegistry::Owner PortRegistry::ownerOf(QString portName) const
{
    // WORKER-FILL: spec §5.5 — 기록된 주인을 돌려주고, 없으면 None이에요.
    Q_UNUSED(portName);
    return Owner::None;
}
}
