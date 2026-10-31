// [0.2.0 T8] specs/features/0.2.0-fcflash.md
#include "FirmwareImage.h"
#include "Constants.h"
#include <kgc/fcFlash/Errors.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QtEndian>
#include <limits>
namespace kgc::fcFlash {
namespace {
kgc::app::errors::Result<QByteArray> unpackImage(const QJsonObject& object, const QString& key)
{
    const auto sizeValue = object.value(key + "_size");
    const double number = sizeValue.toDouble(-1);
    if (number < 0 || number > std::numeric_limits<int>::max() || number != qint64(number))
        return std::unexpected(make(FcFlashError::BadImage));
    const auto size = static_cast<int>(number);
    const auto encoded = object.value(key).toString().toLatin1();
    if (size == 0 && encoded.isEmpty()) return QByteArray{};
    const auto decoded = QByteArray::fromBase64Encoding(encoded, QByteArray::AbortOnBase64DecodingErrors);
    if (!decoded || decoded.decoded.isEmpty()) return std::unexpected(make(FcFlashError::BadImage));
    QByteArray compressed(4, Qt::Uninitialized);
    qToBigEndian<quint32>(size, compressed.data());
    compressed += decoded.decoded;
    const auto bytes = qUncompress(compressed);
    if (bytes.size() != size) return std::unexpected(make(FcFlashError::BadImage));
    return bytes;
}
}
kgc::app::errors::Result<FirmwareImage> parseFirmwareImage(const QByteArray& json)
{
    const auto document = QJsonDocument::fromJson(json);
    if (!document.isObject()) return std::unexpected(make(FcFlashError::BadImage));
    const auto object = document.object();
    const auto magic = object.value("magic").toString();
    if (magic != kMagicPx4 && magic != kMagicArduPilot) return std::unexpected(make(FcFlashError::BadImage));
    FirmwareImage result;
    result.kind = magic == kMagicPx4 ? FirmwareKind::Px4 : FirmwareKind::ArduPilot;
    result.boardId = object.value("board_id").toInt(-1);
    if (result.boardId < 0) return std::unexpected(make(FcFlashError::BadImage));
    const auto image = unpackImage(object, "image");
    if (!image) return std::unexpected(image.error());
    result.image = *image;
    if (object.contains("extf_image") || object.contains("extf_image_size")) {
        const auto external = unpackImage(object, "extf_image");
        if (!external) return std::unexpected(external.error());
        result.extfImage = *external;
    }
    if (result.image.isEmpty() && result.extfImage.isEmpty()) return std::unexpected(make(FcFlashError::BadImage));
    result.summary = object.value("summary").toString();
    if (result.kind == FirmwareKind::Px4) result.expectedVersion = versionInText(object.value("git_identity").toString());
    return result;
}
QString versionInText(const QString& text)
{
    static const QRegularExpression expression(QStringLiteral(R"(\d+\.\d+\.\d+)"));
    return expression.match(text).captured();
}
bool compatible(int boardId, int fileBoardId)
{
    return boardId == fileBoardId || ((boardId == kBoardFmuV2 || boardId == kBoardFmuV3)
        && (fileBoardId == kBoardFmuV2 || fileBoardId == kBoardFmuV3));
}
kgc::app::errors::Result<void> checkImage(const FirmwareImage& image, int boardId, qint64 flashSize, qint64 extfSize)
{
    if (image.image.isEmpty() && image.extfImage.isEmpty()) return std::unexpected(make(FcFlashError::BadImage));
    if (!compatible(boardId, image.boardId)) return std::unexpected(make(FcFlashError::WrongBoard));
    if (image.image.size() > flashSize || image.extfImage.size() > extfSize)
        return std::unexpected(make(FcFlashError::TooLarge));
    return {};
}
}
