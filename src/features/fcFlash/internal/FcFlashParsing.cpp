// [0.2.0 6.8 6.9] specs/features/0.2.0-fcflash.md
#include "FcFlashServicePrivate.h"
#include <QFile>
namespace kgc::fcFlash {
void FcFlashService::Private::cancelPending()
{
    // 이미 큐에 들어간 결과도 선택 번호가 달라지면 현재 선택을 덮어쓰지 못해요.
    ++selectionGeneration;
    if (listRequest) { listRequest->disconnect(q); listRequest->abort(); listRequest.clear(); }
    if (download) { download->disconnect(q); download->abort(); download.clear(); }
    if (!imageBusy && !downloadPath.isEmpty()) QFile::remove(downloadPath);
    downloadPath.clear();
    loading = false;
    imageBusy = false;
}
void FcFlashService::Private::clearSelection()
{
    cancelPending();
    image.reset();
    expected.clear();
    firmwareLabel.clear();
    downloadUrl.clear();
    fileBoardId = 0;
}
void FcFlashService::Private::wireParsing()
{
    qRegisterMetaType<QList<Px4Release>>();
    QObject::connect(worker, &FlashWorker::manifestScanned, q, [this](const auto& entries, int id, quint64 generation) {
        if (generation != selectionGeneration) return;
        manifests.insert(id, entries);
        loading = false;
        if (source == FirmwareSource::ArduPilotOfficial && phase == Phase::Ready) rebuildVersions();
        emit q->changed();
    }, Qt::QueuedConnection);
    QObject::connect(worker, &FlashWorker::manifestFailed, q, [this](quint64 generation) {
        if (generation != selectionGeneration) return;
        loading = false;
        listError = true;
        manifestGzip.clear();
        emit q->changed();
    }, Qt::QueuedConnection);
    QObject::connect(worker, &FlashWorker::releasesParsed, q, [this](const auto& entries, quint64 generation) {
        if (generation != selectionGeneration) return;
        releases = entries;
        loading = false;
        listError = releases.isEmpty();
        if (source == FirmwareSource::Px4Official && phase == Phase::Ready) rebuildVersions();
        emit q->changed();
    }, Qt::QueuedConnection);
    QObject::connect(worker, &FlashWorker::imageParsed, q, [this](auto result, quint64 generation) {
        if (generation == selectionGeneration) imageReady(std::move(result));
    }, Qt::QueuedConnection);
    QObject::connect(worker, &FlashWorker::retryPrepared, q, [this](auto result) {
        if (phase != Phase::Flashing) return;
        if (!result) flashFinished(std::unexpected(result.error()));
        else if (image) runFlash();
        else downloadImage();
    }, Qt::QueuedConnection);
}
}
