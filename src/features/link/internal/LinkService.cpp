// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
#include <kgc/link/LinkService.h>
#include <kgc/comm/links/LinkManager.h>
#include <kgc/comm/links/Errors.h>
namespace kgc::link {
LinkService::LinkService(kgc::comm::links::LinkManager& links,
    kgc::vehicle::registry::VehicleRegistry& vehicles, QObject* parent)
    : QObject(parent), links_(links), vehicles_(vehicles)
{
    connect(&links_, &kgc::comm::links::LinkManager::linkOpened, this, [this] {
        state_ = ConnectionState::Open;
        emit connectionStateChanged();
    });
    connect(&links_, &kgc::comm::links::LinkManager::linkFailed, this,
        [this](kgc::app::errors::ErrorCode error) { finish(error); });
    connect(&links_, &kgc::comm::links::LinkManager::linkClosed, this,
        [this](int, std::optional<kgc::app::errors::ErrorCode> error) { finish(error); });
    connect(&links_, &kgc::comm::links::LinkManager::portsChanged, this, &LinkService::portsChanged);
}
void LinkService::finish(std::optional<kgc::app::errors::ErrorCode> error)
{
    error_ = error;
    state_ = error ? ConnectionState::Failed : ConnectionState::Idle;
    emit connectionStateChanged();
}
bool LinkService::canConnect() const
{
    return state_ == ConnectionState::Idle || state_ == ConnectionState::Failed;
}
void LinkService::connectLink(const kgc::comm::links::LinkConfig& config)
{
    if (!canConnect()) return;
    error_.reset();
    state_ = ConnectionState::Opening;
    emit connectionStateChanged();
    const auto result = links_.connectLink(config);
    if (!result) finish(result.error());
}
void LinkService::disconnectLink() { links_.disconnectLink(); }
QList<QPair<QString, QString>> LinkService::ports() const { return links_.ports(); }
QString LinkService::failureText() const
{
    return error_ ? kgc::comm::links::errorText(*error_) : QString{};
}
}
