// [0.2.0 T4] specs/features/0.2.0-fcflash.md
#include <kgc/comm/rawport/PortRegistry.h>
#include <kgc/comm/rawport/Errors.h>
#include <QHash>
namespace kgc::comm::rawport {
struct PortRegistry::Private {
    struct Entry { Token token; Owner owner; };
    QHash<QString, Entry> entries;
    quint64 nextId = 0;
};
PortRegistry::PortRegistry(QObject* parent) : QObject(parent), d_(std::make_unique<Private>()) {}
PortRegistry::~PortRegistry() = default;
kgc::app::errors::Result<PortRegistry::Token> PortRegistry::claim(QString portName, Owner owner)
{
    if (d_->entries.contains(portName)) return std::unexpected(make(PortError::Busy));
    Q_ASSERT(owner != Owner::None);
    const Token token{++d_->nextId};
    d_->entries.insert(std::move(portName), {token, owner});
    return token;
}
void PortRegistry::release(Token token)
{
    for (auto it = d_->entries.begin(); it != d_->entries.end(); ++it) {
        if (it->token == token) {
            d_->entries.erase(it);
            return;
        }
    }
}
PortRegistry::Owner PortRegistry::ownerOf(QString portName) const
{
    const auto it = d_->entries.constFind(portName);
    return it == d_->entries.cend() ? Owner::None : it->owner;
}
}
