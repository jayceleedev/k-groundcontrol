// [0.2.0 T14–T23] specs/features/0.2.0-fcflash.md
#include "FcFlashServicePrivate.h"
#include "Constants.h"
#include "Events.h"
#include <kgc/app/log/Log.h>
#include <kgc/app/scheduler/Scheduler.h>
#include <kgc/fcFlash/Errors.h>
#include <QCoreApplication>
#include <QFile>
namespace kgc::fcFlash {
FcFlashService::FcFlashService(kgc::comm::links::LinkManager& links, kgc::vehicle::registry::VehicleRegistry& vehicles,
    kgc::vehicle::safety::ArmedPolicy& policy, kgc::comm::rawport::PortRegistry& ports,
    kgc::app::net::Http& http, kgc::app::scheduler::Scheduler& scheduler, QObject* parent)
    : QObject(parent), d_(new Private{this, links, vehicles, policy, ports, http, scheduler})
{
    d_->wire();
}
FcFlashService::~FcFlashService()
{
    d_->abort = true;
    if (d_->listRequest) { d_->listRequest->disconnect(this); d_->listRequest->abort(); }
    if (d_->download) { d_->download->disconnect(this); d_->download->abort(); }
    d_->worker->disconnect(this);
    d_->thread.quit();
    d_->thread.wait();
    d_->leases.clear();
    if (!d_->downloadPath.isEmpty()) QFile::remove(d_->downloadPath);
}
void FcFlashService::Private::wire()
{
    worker = new FlashWorker(abort);
    worker->moveToThread(&thread);
    QObject::connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    wireWorker();
    QObject::connect(&links, &kgc::comm::links::LinkManager::portsChanged, q, &FcFlashService::portsChanged);
    QObject::connect(&links, &kgc::comm::links::LinkManager::linkClosed, q, [this] { linkEnded(); });
    QObject::connect(&links, &kgc::comm::links::LinkManager::linkFailed, q, [this] { linkEnded(); });
    QObject::connect(&vehicles, &kgc::vehicle::registry::VehicleRegistry::vehicleAdded, q, [this](auto* vehicle) {
        heartbeat(vehicle);
        returnVehicle(vehicle);
    });
    QObject::connect(&reopenTimeout, &kgc::app::scheduler::Timeout::expired, q, [this] { tryTemporary(); });
    QObject::connect(&timeout, &kgc::app::scheduler::Timeout::expired, q, [this] { expired(); });
    QObject::connect(&versionTimeout, &kgc::app::scheduler::Timeout::expired, q, [this] { finishReturn(); });
    scheduler.registerTask("fcFlashScan", kScanPeriod, q, q, [this] { scan(); });
    thread.start();
}
void FcFlashService::Private::wireWorker()
{
    qRegisterMetaType<ProbeResult>();
    qRegisterMetaType<BootloaderStage>();
    qRegisterMetaType<kgc::app::errors::Result<void>>();
    qRegisterMetaType<kgc::app::errors::Result<FirmwareImage>>();
    qRegisterMetaType<QList<ProbeResult>>();
    qRegisterMetaType<QList<ManifestEntry>>();
    QObject::connect(worker, &FlashWorker::probed, q, [this](const auto& value) { found = value; }, Qt::QueuedConnection);
    QObject::connect(worker, &FlashWorker::portClosed, q, [this] { portClosed(); }, Qt::QueuedConnection);
    QObject::connect(worker, &FlashWorker::portLost, q, [this] {
        retain.clear();
        fail(make(FcFlashError::PortLost));
    }, Qt::QueuedConnection);
    wireParsing();
    QObject::connect(worker, &FlashWorker::stageChanged, q, [this](BootloaderStage stage) {
        flashStage = static_cast<int>(stage) + 1;
        const char* keys[] = {QT_TRANSLATE_NOOP("Job", "지우기"), QT_TRANSLATE_NOOP("Job", "쓰기"), QT_TRANSLATE_NOOP("Job", "검증")};
        if (job) { job->setStep(keys[static_cast<int>(stage)]); job->setProgress(0); }
        KGC_LOG_INFO(kCategory, FcFlashEvent::StepStarted, "Flash stage started: %1", flashStage);
        emit q->changed();
    }, Qt::QueuedConnection);
    QObject::connect(worker, &FlashWorker::progressChanged, q, [this](double value) {
        if (job) job->setProgress(value);
    }, Qt::QueuedConnection);
    QObject::connect(worker, &FlashWorker::flashFinished, q, [this](auto result) { flashFinished(result); }, Qt::QueuedConnection);
}
void FcFlashService::Private::setPhase(Phase value)
{
    phase = value;
    emit q->changed();
}
void FcFlashService::Private::fail(kgc::app::errors::ErrorCode value, bool canRetry)
{
    error = value;
    retry = canRetry;
    temporaryPending = waitingHeartbeat = false;
    reopenTimeout.stop();
    if (step == Step::Firmware || step == Step::Confirm) step = Step::Board;
    timeout.stop();
    versionTimeout.stop();
    setPhase(Phase::Failed);
}
Step FcFlashService::step() const { return d_->step; }
Phase FcFlashService::phase() const { return d_->phase; }
FirmwareSource FcFlashService::source() const { return d_->source; }
QList<QPair<QString, QString>> FcFlashService::ports() const { return d_->links.ports(); }
int FcFlashService::secondsLeft() const { return d_->seconds; }
bool FcFlashService::canRetry() const { return d_->retry && !d_->closing; }
QStringList FcFlashService::pickPorts() const
{
    QStringList result;
    for (const auto& value : d_->found) result.append(value.portName);
    return result;
}
QString FcFlashService::boardManufacturer() const { return d_->usb.manufacturer; }
QString FcFlashService::boardProduct() const { return d_->usb.product; }
int FcFlashService::boardId() const { return d_->board.boardId; }
int FcFlashService::flashKb() const { return d_->board.flashSize / kBytesPerKb; }
int FcFlashService::bootloaderRevision() const { return d_->board.bootloaderRev; }
QString FcFlashService::vehicle() const { return d_->vehicleName; }
QStringList FcFlashService::versions() const
{
    auto names = d_->versionNames;
    if (!names.isEmpty() && (d_->source == FirmwareSource::Px4Official
        || (!d_->manifestVersionList.isEmpty() && d_->manifestVersionList.first().buildType == "OFFICIAL")))
    {
        const auto format = QT_TRANSLATE_NOOP("FcFlash", "%1 (안정판)");
        names[0] = QCoreApplication::translate("FcFlash", format).arg(names[0]);
    }
    return names;
}
int FcFlashService::versionIndex() const { return d_->version; }
QStringList FcFlashService::candidates() const { return d_->candidateNames; }
int FcFlashService::candidateIndex() const { return d_->candidate; }
bool FcFlashService::listLoading() const { return d_->loading || d_->imageBusy || !d_->download.isNull(); }
bool FcFlashService::listFailed() const { return d_->listError; }
bool FcFlashService::canContinue() const
{
    return d_->phase == Phase::Ready && !d_->closing && !listLoading()
        && (d_->source == FirmwareSource::LocalFile ? d_->image.has_value() : d_->candidate >= 0);
}
QString FcFlashService::filePath() const { return d_->file; }
QString FcFlashService::firmwareName() const { return d_->firmware; }
QString FcFlashService::expectedVersion() const { return d_->expected; }
QString FcFlashService::firmwareBoard() const { return d_->firmwareLabel; }
kgc::app::job::Job* FcFlashService::job() const { return d_->job; }
QString FcFlashService::vehicleVersion() const
{
    return d_->returning ? d_->returning->states()->capabilities().firmwareVersion : d_->actualVersion;
}
QString FcFlashService::vehicleFirmwareName() const
{
    return d_->returning ? d_->returning->states()->capabilities().firmwareName : d_->actualName;
}
int FcFlashService::flashStage() const { return d_->flashStage; }
QString FcFlashService::failureText() const
{
    if (d_->error == make(FcFlashError::WrongBoard)) {
        const auto format = QT_TRANSLATE_NOOP("FcFlash", "이 보드용 펌웨어가 아니에요: 보드 %1, 파일 %2");
        return QCoreApplication::translate("FcFlash", format).arg(d_->board.boardId).arg(d_->fileBoardId);
    }
    return errorText(d_->error);
}
}
