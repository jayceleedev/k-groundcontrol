// [0.2.0 T7] specs/features/0.2.0-fcflash.md
#include "Bootloader.h"
#include <kgc/fcFlash/Errors.h>
namespace kgc::fcFlash {
namespace {
std::unexpected<kgc::app::errors::ErrorCode> notFilled()
{
    return std::unexpected(make(FcFlashError::Aborted));
}
}
Bootloader::Bootloader(kgc::comm::rawport::ByteStream& stream, const std::atomic_bool& abort)
    : stream_(stream), abort_(abort) {}
kgc::app::errors::Result<void> Bootloader::sync()
{
    // WORKER-FILL: spec §6.4 — 0x21 0x20, 0x12 0x10 응답, 300ms씩 3번이에요.
    return notFilled();
}
kgc::app::errors::Result<BoardInfo> Bootloader::identify()
{
    // WORKER-FILL: spec §6.4 — GET_DEVICE 1·2·3·4, 판 5 이상이면 6(실패하면 0). 판이 3~20 밖이면 BadBootloaderRevision이에요.
    return notFilled();
}
kgc::app::errors::Result<void> Bootloader::erase(const ProgressFn& progress)
{
    // WORKER-FILL: spec §6.4 — 시간 제한 20초 + 2000 KB 넘으면 1 MB마다 4초. 읽기 조각마다 경과 비례 진행률을 내요.
    Q_UNUSED(progress);
    return notFilled();
}
kgc::app::errors::Result<void> Bootloader::program(const QByteArray& image, const ProgressFn& progress)
{
    // WORKER-FILL: spec §6.4 — 0xFF로 4바이트 배수까지 채우고 252바이트 묶음으로 보내요. 재시도 없음.
    Q_UNUSED(image);
    Q_UNUSED(progress);
    return notFilled();
}
kgc::app::errors::Result<void> Bootloader::verify(const QByteArray& image)
{
    // WORKER-FILL: spec §6.4 — crc32Bootloader를 image와 0xFF 채움에 플래시 크기까지 이어 셈하고 0x29 응답과 비교해요.
    Q_UNUSED(image);
    return notFilled();
}
kgc::app::errors::Result<void> Bootloader::extfErase(qint64 size, const ProgressFn& progress)
{
    // WORKER-FILL: 조사 §Q4 — 0x34 + u32 LE + 0x20, 동기 응답, 퍼센트 바이트, 100, 동기 응답이에요.
    Q_UNUSED(size);
    Q_UNUSED(progress);
    return notFilled();
}
kgc::app::errors::Result<void> Bootloader::extfProgram(const QByteArray& image, const ProgressFn& progress)
{
    // WORKER-FILL: 조사 §Q4 — 0x35 len data 0x20, 252바이트 묶음이에요.
    Q_UNUSED(image);
    Q_UNUSED(progress);
    return notFilled();
}
kgc::app::errors::Result<void> Bootloader::extfVerify(const QByteArray& image)
{
    // WORKER-FILL: 조사 §Q4 — 0x37 + u32 LE + 0x20, 응답은 u32 CRC와 동기 응답이에요. 채움 없이 비교해요.
    Q_UNUSED(image);
    return notFilled();
}
kgc::app::errors::Result<void> Bootloader::flashImage(const FirmwareImage& image, const StageFn& stage, const ProgressFn& progress)
{
    // WORKER-FILL: spec §6.4 — TooLarge 확인, 외부, 내부 순서예요. image_size가 0이면 내부를 건너뛰어요. boot는 부르지 않아요.
    Q_UNUSED(image);
    Q_UNUSED(stage);
    Q_UNUSED(progress);
    return notFilled();
}
kgc::app::errors::Result<void> Bootloader::boot()
{
    // WORKER-FILL: spec §6.4 — 0x30 0x20을 보내고 응답을 기다리지 않아요.
    return notFilled();
}
}
