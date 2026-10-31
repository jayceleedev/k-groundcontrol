// [0.2.0 T14–T23] specs/features/0.2.0-fcflash.md
#include "FlashWorker.h"
namespace kgc::fcFlash {
struct FlashWorker::Private {
    const std::atomic_bool& abort;
};
FlashWorker::FlashWorker(const std::atomic_bool& abort) : d_(new Private{abort}) {}
FlashWorker::~FlashWorker() = default;
void FlashWorker::probePorts(const QStringList& portNames)
{
    // WORKER-FILL: spec §6.3-9 — 후보마다 RawPort::open, sync, identify를 하고 응답 없는 포트는 닫아요.
    Q_UNUSED(portNames);
}
void FlashWorker::keepPort(const QString& portName)
{
    // WORKER-FILL: spec §6.3-9 — 고른 포트만 남기고 부팅 없이 닫은 뒤 portClosed를 내요.
    Q_UNUSED(portName);
}
void FlashWorker::closePorts()
{
    // WORKER-FILL: spec §6.2 — 쥔 RawPort를 모두 닫고 portClosed를 내요.
}
void FlashWorker::flash(const FirmwareImage& image)
{
    // WORKER-FILL: spec §6.8-3 — Bootloader::flashImage를 부르고 flashFinished를 내요. boot는 부르지 않아요.
    Q_UNUSED(image);
}
void FlashWorker::bootAndClose()
{
    // WORKER-FILL: spec §6.8-4 — Bootloader::boot 뒤 포트를 닫고 portClosed를 내요.
}
void FlashWorker::parseImage(const QString& path)
{
    // WORKER-FILL: spec §6.6 — 파일을 읽어 parseFirmwareImage를 부르고 imageParsed를 내요.
    Q_UNUSED(path);
}
void FlashWorker::parseManifest(const QByteArray& gzipped, int boardId)
{
    // WORKER-FILL: spec §6.5 — gunzip 뒤 scanManifest를 부르고 manifestScanned를 내요.
    Q_UNUSED(gzipped);
    Q_UNUSED(boardId);
}
}
