// [0.4.1 5.5] specs/features/0.4.1-board-match.md
// [0.2.0 6.2–6.9] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/fcFlash/FcFlashService.h>
#include <kgc/comm/links/LinkManager.h>
#include <kgc/comm/rawport/PortLease.h>
#include <kgc/app/net/Http.h>
#include <kgc/app/scheduler/Timeout.h>
#include <kgc/vehicle/registry/VehicleRegistry.h>
#include "FlashWorker.h"
#include "sources/BoardMatch.h"
#include "sources/Px4Releases.h"
#include <QElapsedTimer>
#include <QPointer>
#include <QSet>
#include <QThread>
#include <QVariantList>
#include <functional>
#include <map>
namespace kgc::fcFlash {
struct FcFlashService::Private {
    FcFlashService* q;
    kgc::comm::links::LinkManager& links;
    kgc::vehicle::registry::VehicleRegistry& vehicles;
    kgc::vehicle::safety::ArmedPolicy& policy;
    kgc::comm::rawport::PortRegistry& registry;
    kgc::app::net::Http& http;
    kgc::app::scheduler::Scheduler& scheduler;
    std::atomic_bool abort{false};
    QThread thread{nullptr};
    FlashWorker* worker = nullptr;
    kgc::app::scheduler::Timeout timeout{"fcFlashWait", q};
    kgc::app::scheduler::Timeout versionTimeout{"fcFlashVersion", q};
    kgc::app::scheduler::Timeout reopenTimeout{"fcFlashReopen", q};
    QElapsedTimer clock;
    QElapsedTimer temporaryClock;
    bool temporaryPending = false;
    Step step = Step::Board;
    Phase phase = Phase::Idle;
    FirmwareSource source = FirmwareSource::Px4Official;
    kgc::app::errors::ErrorCode error{};
    BoardInfo board;
    kgc::comm::links::PortInfo usb;
    QString originalPort;
    QString selectedPort;
    QStringList known;
    QStringList known2;
    QHash<QString, qint64> appeared;
    bool disappeared = false;
    bool existingLink = false;
    bool waitingHeartbeat = false;
    bool commandSent = false;
    bool rebootAccepted = false;
    bool waitingLinkClose = false;
    bool initialProbe = false;
    bool fallbackProbe = false;
    bool probeRunning = false;
    bool closing = false;
    bool retry = false;
    int seconds = 0;
    QList<ProbeResult> found;
    std::map<QString, kgc::comm::rawport::PortLease> leases;
    QStringList retain;
    std::function<void()> afterClose;
    QPointer<kgc::vehicle::registry::Vehicle> returning;
    QString actualName;
    QString actualVersion;
    int reconnectLink = -1;
    bool reconnectOpening = false;
    bool versionWaiting = false;
    QList<Px4Release> releases;
    QByteArray manifestGzip;
    QHash<int, ManifestScan> manifests;
    QList<ManifestVersion> manifestVersionList;
    QList<ManifestEntry> manifestCandidateList;
    QList<Px4Candidate> px4CandidateList;
    ArduPilotVehicle vehicleKind = ArduPilotVehicle::Copter;
    QString vehicleName = "Copter";
    QStringList versionNames;
    QStringList candidateNames;
    int version = -1;
    int candidate = -1;
    MatchResult match;
    QVariantList otherBoardList;
    bool matchLogged = false;
    MatchResult loggedMatch;
    QString loggedName;
    bool loading = false;
    bool listError = false;
    bool imageBusy = false;
    quint64 selectionGeneration = 0;
    std::optional<FirmwareImage> image;
    QString file;
    QString expected;
    QString firmware;
    QString firmwareLabel;
    QString downloadUrl;
    QString downloadPath;
    int fileBoardId = 0;
    QPointer<kgc::app::net::HttpRequest> listRequest;
    QPointer<kgc::app::net::HttpRequest> download;
    kgc::app::job::Job* job = nullptr;
    int flashStage = -1;
    void wire();
    void wireWorker();
    void wireParsing();
    void setPhase(Phase value);
    void fail(kgc::app::errors::ErrorCode value, bool canRetry = false);
    QStringList portNames() const;
    void probe(const QStringList& names);
    void portClosed();
    void probeFinished();
    void close(std::function<void()> next, bool boot = false, QString keep = {});
    void ready(const ProbeResult& result);
    void openTemporary();
    void tryTemporary();
    void heartbeat(kgc::vehicle::registry::Vehicle* vehicle);
    void disconnectTemporary(bool accepted);
    void linkEnded();
    void waitReplug();
    void expired();
    void scan();
    void scanBootloader();
    void returnScan();
    void startReturn();
    void returnVehicle(kgc::vehicle::registry::Vehicle* vehicle);
    void finishReturn();
    void rebuildVersions();
    void rebuildCandidates();
    void updateMatch(const QList<QStringList>& names, const QList<OtherBoard>& others);
    void fetchList();
    void cancelPending();
    void clearSelection();
    void downloadImage();
    void imageReady(kgc::app::errors::Result<FirmwareImage> result);
    void newJob();
    void runFlash();
    void flashFinished(kgc::app::errors::Result<void> result);
    void clearSession();
};
}
