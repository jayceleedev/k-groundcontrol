// [0.2.0 T19–T22] specs/features/0.2.0-fcflash.md
#include "FcFlashServicePrivate.h"
#include "Constants.h"
#include "Events.h"
#include <kgc/app/log/Log.h>
#include <kgc/app/paths/Paths.h>
#include <QFile>
#include <QDir>
#include <QUuid>
namespace kgc::fcFlash {
void FcFlashService::Private::newJob()
{
    if (job) job->deleteLater();
    job = new kgc::app::job::Job(kgc::app::job::OperationKind::FcFlash, q);
    QObject::connect(job, &kgc::app::job::Job::cancelRequested, q, &FcFlashService::cancelFlash);
    error = {};
    retry = false;
    step = Step::Flash;
    flashStage = source == FirmwareSource::LocalFile ? 1 : 0;
    setPhase(Phase::Flashing);
}
void FcFlashService::startFlash()
{
    if (d_->step != Step::Confirm || !canContinue()) return;
    d_->newJob();
    if (d_->source == FirmwareSource::LocalFile) { d_->runFlash(); return; }
    d_->downloadImage();
}
void FcFlashService::Private::downloadImage()
{
    job->setStep(QT_TRANSLATE_NOOP("Job", "내려받기"));
    job->setCancellable(true);
    const auto directory = kgc::app::paths::Paths::cacheFirmware();
    if (directory.isEmpty()) { flashFinished(std::unexpected(make(FcFlashError::DownloadFailed))); return; }
    downloadPath = QDir(directory).filePath(QUuid::createUuid().toString(QUuid::WithoutBraces) + ".json");
    const auto generation = selectionGeneration;
    const auto path = downloadPath;
    download = http.download(QUrl(downloadUrl), path);
    QObject::connect(download, &kgc::app::net::HttpRequest::progress, q, [this, generation](qint64 received, qint64 total) {
        if (generation == selectionGeneration && phase == Phase::Flashing)
            job->setProgress(total > 0 ? double(received) / total : -1);
    });
    QObject::connect(download, &kgc::app::net::HttpRequest::finished, q, [this, generation, path](auto result) {
        if (generation != selectionGeneration) { QFile::remove(path); return; }
        download.clear();
        if (phase != Phase::Flashing) { QFile::remove(path); downloadPath.clear(); emit q->changed(); return; }
        if (!result) { flashFinished(std::unexpected(make(FcFlashError::DownloadFailed))); return; }
        imageBusy = true;
        QMetaObject::invokeMethod(worker, [this, path, generation] { worker->parseImage(path, generation, true); }, Qt::QueuedConnection);
    });
}
void FcFlashService::Private::runFlash()
{
    if (!image || phase != Phase::Flashing || closing) return;
    // 워커에 지우기를 예약하기 전에 취소를 닫아, 이미 시작한 쓰기를 화면에서 끊지 못하게 해요.
    job->setCancellable(false);
    job->setStep(QT_TRANSLATE_NOOP("Job", "지우기"));
    job->setProgress(0);
    flashStage = 1;
    abort = false;
    QMetaObject::invokeMethod(worker, [this, value = *image] { worker->flash(value); }, Qt::QueuedConnection);
    emit q->changed();
}
void FcFlashService::Private::flashFinished(kgc::app::errors::Result<void> result)
{
    if (phase != Phase::Flashing) return;
    KGC_LOG_INFO(kCategory, FcFlashEvent::FlashFinished, "Flash finished: %1", result.has_value());
    if (!result) {
        error = result.error();
        job->fail(error, q->failureText());
        if (!downloadPath.isEmpty()) { QFile::remove(downloadPath); downloadPath.clear(); }
        const bool invalidImage = error == make(FcFlashError::WrongBoard) || error == make(FcFlashError::BadImage)
            || error == make(FcFlashError::TooLarge);
        fail(error, !invalidImage && error != make(FcFlashError::PortLost) && !leases.empty());
        return;
    }
    job->succeed();
    flashStage = 4;
    links.refreshPorts();
    known2 = portNames();
    clock.restart();
    close([this] { startReturn(); }, true);
    emit q->changed();
}
void FcFlashService::cancelFlash()
{
    if (d_->phase != Phase::Flashing || !d_->job || !d_->job->cancellable()) return;
    d_->job->finishCancelled();
    d_->step = Step::Confirm;
    d_->setPhase(Phase::Ready);
    d_->cancelPending();
    emit changed();
}
void FcFlashService::retryFlash()
{
    if (d_->phase != Phase::Failed || d_->step != Step::Flash || !canRetry()) return;
    d_->newJob();
    d_->job->setCancellable(false);
    QMetaObject::invokeMethod(d_->worker, [this] { d_->worker->prepareRetry(); }, Qt::QueuedConnection);
}
void FcFlashService::bootAndReset()
{
    if (d_->phase != Phase::Ready || d_->closing) return;
    d_->cancelPending();
    d_->close([this] { d_->clearSession(); }, true);
}
void FcFlashService::reset()
{
    if (d_->phase == Phase::Flashing || d_->phase == Phase::Reconnecting || d_->closing) return;
    d_->cancelPending();
    d_->timeout.stop();
    d_->versionTimeout.stop();
    if (!d_->leases.empty()) d_->close([this] { d_->clearSession(); });
    else d_->clearSession();
}
void FcFlashService::Private::clearSession()
{
    cancelPending();
    timeout.stop();
    versionTimeout.stop();
    reopenTimeout.stop();
    temporaryPending = waitingHeartbeat = commandSent = waitingLinkClose = false;
    step = Step::Board;
    error = {};
    retry = false;
    loading = false;
    listError = false;
    imageBusy = false;
    seconds = 0;
    board = {};
    usb = {};
    selectedPort.clear();
    found.clear();
    returning.clear();
    actualName.clear();
    actualVersion.clear();
    image.reset();
    file.clear();
    candidateNames.clear();
    versionNames.clear();
    candidate = version = -1;
    flashStage = -1;
    firmware.clear();
    expected.clear();
    firmwareLabel.clear();
    if (job) { job->deleteLater(); job = nullptr; }
    setPhase(Phase::Idle);
}
}
