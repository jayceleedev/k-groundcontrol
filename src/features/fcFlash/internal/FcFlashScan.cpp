// [0.2.0 T15–T21] specs/features/0.2.0-fcflash.md
#include "FcFlashServicePrivate.h"
#include "Constants.h"
#include "Events.h"
#include <kgc/app/log/Log.h>
#include <QFile>
namespace kgc::fcFlash {
void FcFlashService::Private::scan()
{
    if (phase == Phase::Reconnecting) { returnScan(); return; }
    if (phase == Phase::Rebooting || phase == Phase::WaitReplug) {
        scanBootloader();
        return;
    }
    if ((phase == Phase::Ready || phase == Phase::Flashing || phase == Phase::Failed)
        && !selectedPort.isEmpty() && !leases.empty() && !closing) {
        links.refreshPorts();
        // 독점으로 연 포트는 목록에서 빠질 수 있어 Ports.cpp처럼 장치 파일도 확인해요.
        if (!portNames().contains(selectedPort)
            && !QFile::exists(selectedPort.startsWith('/') ? selectedPort : QStringLiteral("/dev/") + selectedPort)) {
            abort = true;
            cancelPending();
            if (job && phase == Phase::Flashing) job->fail(make(FcFlashError::PortLost), errorText(make(FcFlashError::PortLost)));
            fail(make(FcFlashError::PortLost));
            close([this] { abort = false; });
        }
    }
}
void FcFlashService::Private::scanBootloader()
{
    links.refreshPorts();
    const auto current = portNames();
    if (!current.contains(originalPort)) disappeared = true;
    if (phase == Phase::WaitReplug) {
        seconds = std::max(qint64(0), (kBootloaderWait.count() - clock.elapsed() + 1000 - 1) / 1000);
        emit q->changed();
    }
    for (auto it = appeared.begin(); it != appeared.end();) {
        if (!current.contains(it.key())) it = appeared.erase(it);
        else ++it;
    }
    QStringList candidates;
    for (const auto& name : current) {
        if (known.contains(name) && !(name == originalPort && disappeared)) continue;
        if (!appeared.contains(name)) appeared.insert(name, clock.elapsed());
        if (clock.elapsed() - appeared.value(name) >= kPortSettle.count()) candidates.append(name);
    }
    if (!probeRunning && !closing && !candidates.isEmpty()) probe(candidates);
}
void FcFlashService::Private::startReturn()
{
    flashStage = 5;
    reconnectLink = -1;
    reconnectOpening = false;
    returning.clear();
    versionWaiting = false;
    disappeared = false;
    timeout.start(kReconnectWait);
    setPhase(Phase::Reconnecting);
}
void FcFlashService::Private::returnScan()
{
    links.refreshPorts();
    if (links.linkId() >= 0 && links.linkId() != reconnectLink) {
        timeout.stop();
        versionTimeout.stop();
        setPhase(Phase::NoReturn);
        return;
    }
    if (reconnectOpening) { returnVehicle(vehicles.active()); return; }
    const auto current = portNames();
    if (!current.contains(originalPort)) disappeared = true;
    QStringList candidates;
    if (current.contains(originalPort)) {
        if (disappeared || clock.elapsed() >= kReturnGrace.count()) candidates.append(originalPort);
    } else {
        for (const auto& name : current)
            if (!known2.contains(name)) candidates.append(name);
    }
    if (candidates.size() > 1) {
        timeout.stop();
        setPhase(Phase::NoReturn);
        return;
    }
    if (candidates.isEmpty()) return;
    const auto result = links.connectLink({kgc::comm::links::LinkConfig::Kind::Serial, candidates.first(), kFlashBaud});
    if (result) {
        reconnectLink = links.linkId();
        reconnectOpening = true;
    }
}
void FcFlashService::Private::returnVehicle(kgc::vehicle::registry::Vehicle* value)
{
    if (phase != Phase::Reconnecting || versionWaiting || !reconnectOpening || links.linkId() != reconnectLink) return;
    if (!value) return;
    returning = value;
    versionWaiting = true;
    timeout.stop();
    QObject::connect(value->states(), &kgc::vehicle::states::VehicleStates::capabilitiesChanged, q, [this] {
        emit q->changed();
    });
    versionTimeout.start(kVersionWait);
}
void FcFlashService::Private::finishReturn()
{
    if (phase != Phase::Reconnecting || !returning) return;
    const auto capabilities = returning->states()->capabilities();
    actualName = capabilities.firmwareName;
    actualVersion = capabilities.firmwareVersion;
    const bool mismatch = actualName != firmware || (!expected.isEmpty() && !actualVersion.isEmpty() && expected != actualVersion);
    timeout.stop();
    versionTimeout.stop();
    flashStage = 6;
    setPhase(mismatch ? Phase::Mismatch : Phase::Done);
    KGC_LOG_INFO(kCategory, FcFlashEvent::ReconnectFinished, "Reconnect finished: %1 %2, mismatch: %3", actualName, actualVersion, mismatch);
}
}
