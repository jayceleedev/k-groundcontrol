// [0.2.0 T14–T23] specs/features/0.2.0-fcflash.md
#include "FlashWorker.h"
#include "Constants.h"
#include <kgc/comm/rawport/RawPort.h>
#include <kgc/app/net/Http.h>
#include <QFile>
#include <map>
namespace kgc::fcFlash {
struct FlashWorker::Private {
    const std::atomic_bool& abort;
    std::map<QString, std::unique_ptr<kgc::comm::rawport::RawPort>> ports;
};
FlashWorker::FlashWorker(const std::atomic_bool& abort) : d_(new Private{abort, {}}) {}
FlashWorker::~FlashWorker() = default;
void FlashWorker::probePorts(const QStringList& portNames)
{
    QList<ProbeResult> found;
    for (const auto& name : portNames) {
        if (d_->abort) break;
        auto port = kgc::comm::rawport::RawPort::open(name, kFlashBaud);
        if (!port) continue;
        Bootloader loader(**port, d_->abort);
        if (!loader.sync()) continue;
        const auto board = loader.identify();
        if (!board) {
            if (board.error() == make(FcFlashError::BadBootloaderRevision)) found.append({name, {}, board.error()});
            continue;
        }
        d_->ports.emplace(name, std::move(*port));
        found.append({name, *board});
    }
    // 응답하지 않은 후보도 닫은 뒤에 알려야 서비스가 임대를 안전하게 놓을 수 있어요.
    emit probed(found);
    emit portClosed();
}
void FlashWorker::keepPort(const QString& portName)
{
    std::erase_if(d_->ports, [&](const auto& entry) { return entry.first != portName; });
    emit portClosed();
}
void FlashWorker::closePorts()
{
    d_->ports.clear();
    emit portClosed();
}
void FlashWorker::flash(const FirmwareImage& image)
{
    if (d_->ports.empty()) {
        emit flashFinished(std::unexpected(make(FcFlashError::PortLost)));
        return;
    }
    Bootloader loader(*d_->ports.begin()->second, d_->abort);
    auto result = loader.sync();
    if (result) {
        const auto board = loader.identify();
        if (!board) result = std::unexpected(board.error());
        else result = loader.flashImage(image,
            [this](BootloaderStage stage) { emit stageChanged(stage); },
            [this](double value) { emit progressChanged(value); });
    }
    emit flashFinished(result);
    if (!result && result.error() == make(FcFlashError::PortLost)) {
        d_->ports.clear();
        emit portLost();
        emit portClosed();
    }
}
void FlashWorker::bootAndClose()
{
    for (const auto& [name, port] : d_->ports) {
        Bootloader loader(*port, d_->abort);
        loader.boot();
    }
    closePorts();
}
void FlashWorker::parseImage(const QString& path, quint64 generation, bool temporary)
{
    QFile file(path);
    auto result = file.open(QIODevice::ReadOnly) ? parseFirmwareImage(file.readAll())
        : kgc::app::errors::Result<FirmwareImage>(std::unexpected(make(FcFlashError::BadImage)));
    file.close();
    if (temporary) QFile::remove(path);
    emit imageParsed(std::move(result), generation);
}
void FlashWorker::parseManifest(const QByteArray& gzipped, int boardId, quint64 generation)
{
    const auto bytes = kgc::app::net::gunzip(gzipped);
    if (!bytes) emit manifestFailed(generation);
    else emit manifestScanned(scanManifest(*bytes, boardId), boardId, generation);
}
void FlashWorker::parseReleases(const QByteArray& json, quint64 generation)
{
    emit releasesParsed(parsePx4Releases(json), generation);
}
void FlashWorker::prepareRetry()
{
    kgc::app::errors::Result<void> result;
    if (d_->ports.empty()) result = std::unexpected(make(FcFlashError::PortLost));
    else {
        Bootloader loader(*d_->ports.begin()->second, d_->abort);
        result = loader.sync();
        if (result) {
            const auto board = loader.identify();
            if (!board) result = std::unexpected(board.error());
        }
    }
    emit retryPrepared(result);
}
}
