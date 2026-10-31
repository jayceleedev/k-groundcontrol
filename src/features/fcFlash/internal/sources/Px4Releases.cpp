// [0.2.0 T10] specs/features/0.2.0-fcflash.md
#include "Px4Releases.h"
#include "Constants.h"
#include "image/FirmwareImage.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVersionNumber>
#include <algorithm>
namespace kgc::fcFlash {
QList<Px4Release> parsePx4Releases(const QByteArray& json)
{
    QList<Px4Release> result;
    for (const auto value : QJsonDocument::fromJson(json).array()) {
        const auto object = value.toObject();
        if (object.value("draft").toBool() || object.value("prerelease").toBool()) continue;
        Px4Release release;
        release.tag = object.value("tag_name").toString();
        release.version = versionInText(release.tag);
        if (release.version.isEmpty()) continue;
        for (const auto assetValue : object.value("assets").toArray()) {
            const auto asset = assetValue.toObject();
            release.assets.append({asset.value("name").toString(), asset.value("browser_download_url").toString()});
        }
        result.append(release);
    }
    std::stable_sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
        return QVersionNumber::fromString(a.version) > QVersionNumber::fromString(b.version);
    });
    return result;
}
QStringList px4Prefixes(const QByteArray& boardsJson, int boardId)
{
    if (boardId == kBoardFmuV3) boardId = kBoardFmuV2;
    QStringList result;
    for (const auto value : QJsonDocument::fromJson(boardsJson).array()) {
        const auto object = value.toObject();
        const auto prefix = object.value("prefix").toString();
        if (object.value("boardId").toInt() == boardId && !prefix.isEmpty() && !result.contains(prefix)) result.append(prefix);
    }
    return result;
}
QList<Px4Candidate> px4Candidates(const Px4Release& release, const QStringList& prefixes)
{
    QList<Px4Candidate> result;
    for (const auto& asset : release.assets) {
        for (const auto& prefix : prefixes) {
            if (!asset.name.startsWith(prefix + '_') || !asset.name.endsWith(".px4")) continue;
            const auto label = asset.name.mid(prefix.size() + 1).chopped(4);
            if (label.isEmpty() || label == kBootloaderLabel) continue;
            result.append({prefix, label, asset.url});
        }
    }
    return result;
}
}
