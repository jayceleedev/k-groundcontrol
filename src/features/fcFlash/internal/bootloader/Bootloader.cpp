// [0.2.0 T7] specs/features/0.2.0-fcflash.md
#include "Bootloader.h"
#include "Constants.h"
#include <kgc/app/crc/Crc.h>
#include <kgc/comm/rawport/Errors.h>
#include <QElapsedTimer>
#include <QtEndian>
namespace kgc::fcFlash {
using kgc::app::errors::Result;
Bootloader::Bootloader(kgc::comm::rawport::ByteStream& stream, const std::atomic_bool& abort)
    : stream_(stream), abort_(abort) {}
Result<void> Bootloader::send(const QByteArray& bytes, FcFlashError failure)
{
    if (abort_) return std::unexpected(make(FcFlashError::Aborted));
    const auto result = stream_.write(bytes);
    if (result) return {};
    using namespace kgc::comm::rawport;
    return std::unexpected(make(result.error() == make(PortError::Lost) ? FcFlashError::PortLost : failure));
}
Result<QByteArray> Bootloader::receive(int count, std::chrono::milliseconds timeout,
    FcFlashError failure, const ProgressFn& progress)
{
    QElapsedTimer clock;
    clock.start();
    QByteArray bytes;
    while (bytes.size() < count && clock.elapsed() < timeout.count()) {
        if (abort_) return std::unexpected(make(FcFlashError::Aborted));
        const auto remaining = timeout - std::chrono::milliseconds(clock.elapsed());
        const auto part = stream_.read(count - bytes.size(), std::min(kReadSlice, remaining));
        if (part) bytes += *part;
        else if (part.error() != kgc::comm::rawport::make(kgc::comm::rawport::PortError::Timeout))
            return std::unexpected(make(FcFlashError::PortLost));
        if (progress) progress(std::min(1.0, double(clock.elapsed()) / timeout.count()));
    }
    if (abort_) return std::unexpected(make(FcFlashError::Aborted));
    if (bytes.size() != count) return std::unexpected(make(failure));
    return bytes;
}
Result<QByteArray> Bootloader::reply(int count, std::chrono::milliseconds timeout,
    FcFlashError failure, const ProgressFn& progress)
{
    const auto bytes = receive(count + 2, timeout, failure, progress);
    if (!bytes) return std::unexpected(bytes.error());
    if (bytes->at(count) != kInSync || bytes->at(count + 1) != kOk)
        return std::unexpected(make(failure));
    return bytes->first(count);
}
Result<void> Bootloader::sync()
{
    for (int attempt = 0; attempt < kSyncTries; ++attempt) {
        stream_.clearInput();
        const auto sent = send((QByteArray(1, kGetSync) + kEoc), FcFlashError::BootloaderNotFound);
        if (!sent) return sent;
        const auto result = reply(0, kSyncTimeout, FcFlashError::BootloaderNotFound);
        if (result) return {};
        if (result.error() != make(FcFlashError::BootloaderNotFound)) return std::unexpected(result.error());
    }
    return std::unexpected(make(FcFlashError::BootloaderNotFound));
}
Result<BoardInfo> Bootloader::identify()
{
    const auto device = [this](char arg) -> Result<quint32> {
        const auto sent = send((QByteArray(1, kGetDevice) + arg + kEoc), FcFlashError::BootloaderNotFound);
        if (!sent) return std::unexpected(sent.error());
        const auto bytes = reply(4, kDeviceTimeout, FcFlashError::BootloaderNotFound);
        if (!bytes) return std::unexpected(bytes.error());
        return qFromLittleEndian<quint32>(bytes->constData());
    };
    const auto revision = device(kInfoBootloaderRev);
    if (!revision) return std::unexpected(revision.error());
    if (*revision < kMinBootloaderRev || *revision > kMaxBootloaderRev)
        return std::unexpected(make(FcFlashError::BadBootloaderRevision));
    board_ = {};
    board_.bootloaderRev = *revision;
    for (const char arg : {kInfoBoardId, kInfoBoardRev, kInfoFlashSize}) {
        const auto value = device(arg);
        if (!value) return std::unexpected(value.error());
        if (arg == kInfoBoardId) board_.boardId = *value;
        else if (arg == kInfoBoardRev) board_.boardRev = *value;
        else board_.flashSize = *value;
    }
    if (*revision >= kMinExtfBootloaderRev) {
        const auto value = device(kInfoExtfSize);
        if (value) board_.extfSize = *value;
        else if (value.error() == make(FcFlashError::Aborted) || value.error() == make(FcFlashError::PortLost))
            return std::unexpected(value.error());
    }
    return board_;
}
Result<void> Bootloader::erase(const ProgressFn& progress)
{
    const auto sent = send((QByteArray(1, kChipErase) + kEoc), FcFlashError::EraseFailed);
    if (!sent) return sent;
    const auto extra = board_.flashSize > kEraseExtraAboveBytes ? board_.flashSize / kBytesPerMb : 0;
    const auto result = reply(0, kEraseTimeout + extra * kEraseExtraPerMb, FcFlashError::EraseFailed, progress);
    if (!result) return std::unexpected(result.error());
    if (progress) progress(1.0);
    return {};
}
Result<void> Bootloader::programChunks(char command, const QByteArray& image, const ProgressFn& progress)
{
    QByteArray padded = image;
    padded.append((4 - padded.size() % 4) % 4, kPadByte);
    for (qsizetype offset = 0; offset < padded.size(); offset += kProgramChunk) {
        const auto chunk = padded.mid(offset, kProgramChunk);
        const auto sent = send((QByteArray(1, command) + char(chunk.size())) + chunk + kEoc, FcFlashError::ProgramFailed);
        if (!sent) return sent;
        const auto result = reply(0, kProgramTimeout, FcFlashError::ProgramFailed);
        if (!result) return std::unexpected(result.error());
        if (progress) progress(double(offset + chunk.size()) / padded.size());
    }
    return {};
}
Result<void> Bootloader::program(const QByteArray& image, const ProgressFn& progress)
{
    return programChunks(kProgMulti, image, progress);
}
Result<void> Bootloader::verify(const QByteArray& image)
{
    const auto sent = send((QByteArray(1, kGetCrc) + kEoc), FcFlashError::VerifyFailed);
    if (!sent) return sent;
    const auto result = reply(4, kVerifyTimeout, FcFlashError::VerifyFailed);
    if (!result) return std::unexpected(result.error());
    auto crc = kgc::app::crc::crc32Bootloader(image);
    const QByteArray padding(kProgramChunk, kPadByte);
    for (qint64 offset = image.size(); offset < board_.flashSize; offset += padding.size()) {
        if (abort_) return std::unexpected(make(FcFlashError::Aborted));
        crc = kgc::app::crc::crc32Bootloader(QByteArrayView(padding).first(std::min(qint64(padding.size()), board_.flashSize - offset)), crc);
    }
    if (qFromLittleEndian<quint32>(result->constData()) != crc)
        return std::unexpected(make(FcFlashError::VerifyFailed));
    return {};
}
Result<void> Bootloader::extfErase(qint64 size, const ProgressFn& progress)
{
    QByteArray command(5, Qt::Uninitialized);
    command[0] = kExtfErase;
    qToLittleEndian<quint32>(size, command.data() + 1);
    const auto sent = send(command + kEoc, FcFlashError::EraseFailed);
    if (!sent) return sent;
    QElapsedTimer clock;
    clock.start();
    const auto first = reply(0, kExtfEraseTimeout, FcFlashError::EraseFailed);
    if (!first) return std::unexpected(first.error());
    while (clock.elapsed() < kExtfEraseTimeout.count()) {
        const auto value = receive(1, kExtfEraseTimeout - std::chrono::milliseconds(clock.elapsed()), FcFlashError::EraseFailed);
        if (!value) return std::unexpected(value.error());
        const auto percent = static_cast<unsigned char>(value->at(0));
        if (percent > kFullPercent) return std::unexpected(make(FcFlashError::EraseFailed));
        if (progress) progress(double(percent) / kFullPercent);
        if (percent == kFullPercent) {
            const auto last = reply(0, kExtfEraseTimeout - std::chrono::milliseconds(clock.elapsed()), FcFlashError::EraseFailed);
            if (!last) return std::unexpected(last.error());
            return {};
        }
    }
    return std::unexpected(make(FcFlashError::EraseFailed));
}
Result<void> Bootloader::extfProgram(const QByteArray& image, const ProgressFn& progress)
{
    return programChunks(kExtfProgMulti, image, progress);
}
Result<void> Bootloader::extfVerify(const QByteArray& image)
{
    QByteArray command(5, Qt::Uninitialized);
    command[0] = kExtfGetCrc;
    qToLittleEndian<quint32>(image.size(), command.data() + 1);
    const auto sent = send(command + kEoc, FcFlashError::VerifyFailed);
    if (!sent) return sent;
    const auto result = reply(4, kExtfVerifyTimeout, FcFlashError::VerifyFailed);
    if (!result) return std::unexpected(result.error());
    if (qFromLittleEndian<quint32>(result->constData()) != kgc::app::crc::crc32Bootloader(image))
        return std::unexpected(make(FcFlashError::VerifyFailed));
    return {};
}
Result<void> Bootloader::flashImage(const FirmwareImage& image, const StageFn& stage, const ProgressFn& progress)
{
    const auto checked = checkImage(image, board_.boardId, board_.flashSize, board_.extfSize);
    if (!checked) return checked;
    for (const bool external : {true, false}) {
        const auto& bytes = external ? image.extfImage : image.image;
        if (bytes.isEmpty()) continue;
        if (stage) stage(BootloaderStage::Erase);
        auto result = external ? extfErase(bytes.size(), progress) : erase(progress);
        if (!result) return result;
        if (stage) stage(BootloaderStage::Program);
        result = external ? extfProgram(bytes, progress) : program(bytes, progress);
        if (!result) return result;
        if (stage) stage(BootloaderStage::Verify);
        result = external ? extfVerify(bytes) : verify(bytes);
        if (!result) return result;
        if (progress) progress(1.0);
    }
    return {};
}
Result<void> Bootloader::boot()
{
    return send((QByteArray(1, kReboot) + kEoc), FcFlashError::PortLost);
}
}
