// [0.2.0 T8] specs/features/0.2.0-fcflash.md
#include "FirmwareImage.h"
#include <kgc/fcFlash/Errors.h>
namespace kgc::fcFlash {
kgc::app::errors::Result<FirmwareImage> parseFirmwareImage(const QByteArray& json)
{
    // WORKER-FILL: spec §6.6 — magic 판별, base64 + zlib 풀기(qUncompress는 4바이트 길이를 앞에 붙여 써요), 길이 확인, 버전 추출.
    Q_UNUSED(json);
    return std::unexpected(make(FcFlashError::BadImage));
}
QString versionInText(const QString& text)
{
    // WORKER-FILL: spec §6.6 — \d+\.\d+\.\d+의 첫 일치를 돌려줘요.
    Q_UNUSED(text);
    return {};
}
bool compatible(int boardId, int fileBoardId)
{
    // WORKER-FILL: spec §6.6 — 같으면 참이고, 9와 255는 서로 참이에요.
    Q_UNUSED(boardId);
    Q_UNUSED(fileBoardId);
    return false;
}
kgc::app::errors::Result<void> checkImage(const FirmwareImage& image, int boardId, qint64 flashSize, qint64 extfSize)
{
    // WORKER-FILL: spec §6.6 — compatible이 거짓이면 WrongBoard, 내부·외부가 플래시를 넘으면 TooLarge예요.
    Q_UNUSED(image);
    Q_UNUSED(boardId);
    Q_UNUSED(flashSize);
    Q_UNUSED(extfSize);
    return std::unexpected(make(FcFlashError::BadImage));
}
}
