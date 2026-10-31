// [0.2.0 T14–T18] specs/features/0.2.0-fcflash.md
#include "FcFlashServicePrivate.h"
#include "Constants.h"
#include "Events.h"
#include <kgc/app/log/Log.h>
#include <kgc/vehicle/safety/ArmedPolicy.h>
#include <kgc/vehicle/command/CommandQueue.h>
namespace kgc::fcFlash {
QStringList FcFlashService::Private::portNames() const
{
    QStringList result;
    for (const auto& port : links.ports()) result.append(port.first);
    return result;
}
void FcFlashService::beginProbe(const QString& portName)
{
    if (d_->closing || d_->probeRunning || d_->step != Step::Board
        || (d_->phase != Phase::Idle && d_->phase != Phase::Failed) || portName.isEmpty()) return;
    using namespace kgc::vehicle::safety;
    d_->originalPort = portName;
    if (d_->policy.check(kgc::app::job::OperationKind::FcFlash, VehicleScope::Any()) == Decision::Deny) {
        d_->fail(make(FcFlashError::Armed), true);
        return;
    }
    const auto config = d_->links.linkConfig();
    if (d_->links.linkId() >= 0 && (config.kind != kgc::comm::links::LinkConfig::Kind::Serial || config.portName != portName)) {
        d_->fail(make(FcFlashError::OtherLink), true);
        return;
    }
    d_->abort = false;
    d_->error = {};
    d_->retry = false;
    d_->disappeared = false;
    d_->appeared.clear();
    d_->links.refreshPorts();
    d_->known = d_->portNames();
    d_->existingLink = d_->links.linkId() >= 0;
    d_->initialProbe = !d_->existingLink;
    d_->fallbackProbe = false;
    d_->clock.start();
    d_->setPhase(Phase::Probing);
    KGC_LOG_INFO(kCategory, FcFlashEvent::StepStarted, "Board probe started: %1", portName);
    if (d_->existingLink) d_->openTemporary();
    else d_->probe({portName});
}
void FcFlashService::Private::probe(const QStringList& names)
{
    QStringList claimed;
    for (const auto& name : names) {
        auto lease = kgc::comm::rawport::PortLease::acquire(registry, name);
        if (lease) {
            leases.emplace(name, std::move(*lease));
            claimed.append(name);
        }
    }
    probeRunning = true;
    found.clear();
    QMetaObject::invokeMethod(worker, [this, claimed] { worker->probePorts(claimed); }, Qt::QueuedConnection);
}
void FcFlashService::Private::portClosed()
{
    if (probeRunning) {
        probeRunning = false;
        if (closing) return;
        retain.clear();
        for (const auto& value : found) if (value.error == kgc::app::errors::ErrorCode{}) retain.append(value.portName);
        std::erase_if(leases, [this](const auto& entry) { return !retain.contains(entry.first); });
        probeFinished();
        return;
    }
    std::erase_if(leases, [this](const auto& entry) { return !retain.contains(entry.first); });
    closing = false;
    if (afterClose) {
        auto next = std::move(afterClose);
        afterClose = {};
        next();
    }
    emit q->changed();
}
void FcFlashService::Private::probeFinished()
{
    if (phase != Phase::Probing && phase != Phase::Rebooting && phase != Phase::WaitReplug) {
        close([] {});
        return;
    }
    for (const auto& value : found) {
        if (value.error != make(FcFlashError::BadBootloaderRevision)) continue;
        fail(value.error);
        close([] {});
        return;
    }
    if (!found.isEmpty()) {
        timeout.stop();
        initialProbe = false;
        fallbackProbe = false;
        if (found.size() == 1) ready(found.first());
        else setPhase(Phase::PickPort);
    } else if (initialProbe) {
        initialProbe = false;
        openTemporary();
    } else if (fallbackProbe) {
        fallbackProbe = false;
        waitReplug();
    }
}
void FcFlashService::Private::close(std::function<void()> next, bool boot, QString keep)
{
    closing = true;
    retain = keep.isEmpty() ? QStringList{} : QStringList{keep};
    afterClose = std::move(next);
    QMetaObject::invokeMethod(worker, [this, boot, keep] {
        if (boot) worker->bootAndClose();
        else if (!keep.isEmpty()) worker->keepPort(keep);
        else worker->closePorts();
    }, Qt::QueuedConnection);
}
void FcFlashService::Private::ready(const ProbeResult& result)
{
    selectedPort = result.portName;
    board = result.board;
    usb = links.portInfo(selectedPort);
    timeout.stop();
    step = Step::Firmware;
    error = {};
    retry = false;
    setPhase(Phase::Ready);
    KGC_LOG_INFO(kCategory, FcFlashEvent::BoardIdentified, "Board identified: %1, revision: %2, flash: %3", board.boardId, board.bootloaderRev, board.flashSize);
    q->chooseSource(FirmwareSource::Px4Official);
}
void FcFlashService::pickPort(const QString& portName)
{
    if (d_->phase != Phase::PickPort || d_->closing) return;
    for (const auto& value : d_->found) {
        if (value.portName != portName) continue;
        d_->close([this, value] { d_->ready(value); }, false, portName);
        return;
    }
}
void FcFlashService::retryProbe()
{
    if (canRetry() && d_->step == Step::Board) beginProbe(d_->originalPort);
}
void FcFlashService::Private::openTemporary()
{
    waitingHeartbeat = true;
    commandSent = false;
    rebootAccepted = false;
    temporaryPending = links.linkId() < 0;
    temporaryClock.start();
    timeout.start(kHeartbeatWait);
    if (temporaryPending) reopenTimeout.start(kPortSettle);
    else heartbeat(vehicles.active());
}
void FcFlashService::Private::tryTemporary()
{
    if (!temporaryPending || phase != Phase::Probing || links.linkId() >= 0
        || temporaryClock.elapsed() >= kHeartbeatWait.count()) return;
    const auto result = links.connectLink({kgc::comm::links::LinkConfig::Kind::Serial, originalPort, kFlashBaud});
    if (result) temporaryPending = false;
    else reopenTimeout.start(kPortSettle);
}
void FcFlashService::Private::heartbeat(kgc::vehicle::registry::Vehicle* vehicle)
{
    if (!waitingHeartbeat || commandSent || !vehicle) return;
    using namespace kgc::vehicle::safety;
    waitingHeartbeat = false;
    temporaryPending = false;
    timeout.stop();
    if (policy.check(kgc::app::job::OperationKind::FcFlash, VehicleScope::One(vehicle)) == Decision::Deny) {
        fail(make(FcFlashError::Armed), true);
        if (!existingLink) links.disconnectLink();
        return;
    }
    commandSent = true;
    kgc::vehicle::command::CommandLong command;
    command.command = MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN;
    command.param1 = kRebootToBootloader;
    vehicle->commands()->send(command, {.retries = 0}, [this](auto result) {
        if (phase == Phase::Probing) disconnectTemporary(result.result == MAV_RESULT_ACCEPTED);
    }, q);
}
void FcFlashService::Private::disconnectTemporary(bool accepted)
{
    waitingHeartbeat = false;
    temporaryPending = false;
    reopenTimeout.stop();
    rebootAccepted = accepted;
    waitingLinkClose = true;
    timeout.stop();
    if (links.linkId() >= 0) links.disconnectLink();
    else linkEnded();
}
void FcFlashService::Private::linkEnded()
{
    if (phase == Phase::Reconnecting) {
        reconnectOpening = false;
        reconnectLink = -1;
        returning.clear();
        versionWaiting = false;
        versionTimeout.stop();
        const auto remaining = kReconnectWait - std::chrono::milliseconds(clock.elapsed());
        if (remaining.count() > 0) timeout.start(remaining);
        else setPhase(Phase::NoReturn);
        return;
    }
    if (phase != Phase::Probing) return;
    if (!existingLink && waitingHeartbeat && !waitingLinkClose && !commandSent) {
        temporaryPending = true;
        reopenTimeout.start(kPortSettle);
        return;
    }
    if (!waitingLinkClose && !waitingHeartbeat && !commandSent) return;
    waitingLinkClose = false;
    waitingHeartbeat = false;
    timeout.stop();
    if (rebootAccepted) {
        clock.restart();
        appeared.clear();
        timeout.start(kRebootWait);
        setPhase(Phase::Rebooting);
    } else if (existingLink) {
        existingLink = false;
        fallbackProbe = true;
        probe({originalPort});
    } else waitReplug();
}
void FcFlashService::Private::waitReplug()
{
    clock.restart();
    appeared.clear();
    seconds = kBootloaderWait.count() / 1000;
    timeout.start(kBootloaderWait);
    setPhase(Phase::WaitReplug);
}
void FcFlashService::Private::expired()
{
    if (phase == Phase::Probing && waitingHeartbeat) disconnectTemporary(false);
    else if (phase == Phase::Rebooting) waitReplug();
    else if (phase == Phase::WaitReplug) {
        abort = true;
        fail(make(FcFlashError::BootloaderNotFound), true);
        close([this] { abort = false; });
    } else if (phase == Phase::Reconnecting && !versionWaiting) {
        versionTimeout.stop();
        setPhase(Phase::NoReturn);
    }
}
}
