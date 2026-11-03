// [0.4.0 5.2-2] specs/features/0.4.0-sitl.md
// [0.2.0 T22] specs/features/0.2.0-fcflash.md
// [0.1.1 T6 T14 T17] specs/features/0.1.1-link-hardening.md
// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
#include <kgc/link/LinkService.h>
#include <kgc/comm/links/LinkManager.h>
#include <kgc/comm/links/Errors.h>
#include <kgc/vehicle/registry/VehicleRegistry.h>
#include <kgc/app/scheduler/Scheduler.h>
#include "Constants.h"
namespace kgc::link {
bool noVehicleTooLong(qint64 openedMs, qint64 nowMs, bool hasVehicle)
{
    return openedMs >= 0 && !hasVehicle && nowMs - openedMs >= kNoVehicleTimeout.count();
}
LinkService::LinkService(kgc::comm::links::LinkManager& links,
    kgc::vehicle::registry::VehicleRegistry& vehicles, kgc::app::scheduler::Scheduler& scheduler, QObject* parent)
    : QObject(parent), links_(links), vehicles_(vehicles)
{
    clock_.start();
    connect(&links_, &kgc::comm::links::LinkManager::linkOpened, this, [this] {
        openedMs_ = clock_.elapsed();
        checkNoVehicle();
        state_ = ConnectionState::Open;
        emit connectionStateChanged();
    });
    connect(&links_, &kgc::comm::links::LinkManager::linkFailed, this,
        [this](kgc::app::errors::ErrorCode error) { finish(error); });
    connect(&links_, &kgc::comm::links::LinkManager::linkClosed, this,
        [this](int, std::optional<kgc::app::errors::ErrorCode> error) { finish(error); });
    connect(&links_, &kgc::comm::links::LinkManager::portsChanged, this, &LinkService::portsChanged);
    connect(&vehicles_, &kgc::vehicle::registry::VehicleRegistry::activeChanged, this, &LinkService::checkNoVehicle);
    scheduler.registerTask("linkNoVehicle", kNoVehicleCheckPeriod, this, this, [this] { checkNoVehicle(); });
}
void LinkService::finish(std::optional<kgc::app::errors::ErrorCode> error)
{
    openedMs_ = -1;
    checkNoVehicle();
    error_ = error;
    state_ = error ? ConnectionState::Failed : ConnectionState::Idle;
    emit connectionStateChanged();
}
bool LinkService::canConnect() const
{
    // 다른 모듈이 연 링크가 아직 열리는 중이어도 연결 단추가 끊기로 동작하게 해요.
    return (state_ == ConnectionState::Idle || state_ == ConnectionState::Failed) && links_.linkId() < 0;
}
void LinkService::connectLink(const kgc::comm::links::LinkConfig& config)
{
    if (!canConnect()) return;
    error_.reset();
    state_ = ConnectionState::Opening;
    const auto result = links_.connectLink(config);
    if (!result) finish(result.error());
    else emit connectionStateChanged();
}
void LinkService::checkNoVehicle()
{
    const auto value = noVehicleTooLong(openedMs_, clock_.elapsed(), vehicles_.active() != nullptr);
    if (noVehicle_ == value) return;
    noVehicle_ = value;
    emit noVehicleChanged();
}
QString LinkService::linkPortName() const
{
    return links_.linkId() >= 0 && serialLink() ? links_.linkConfig().portName : QString{};
}
bool LinkService::serialLink() const
{
    return links_.linkConfig().kind == kgc::comm::links::LinkConfig::Kind::Serial;
}
void LinkService::disconnectLink() { links_.disconnectLink(); }
QList<QPair<QString, QString>> LinkService::ports() const { return links_.ports(); }
QString LinkService::failureText() const
{
    return error_ ? kgc::comm::links::errorText(*error_) : QString{};
}
}
