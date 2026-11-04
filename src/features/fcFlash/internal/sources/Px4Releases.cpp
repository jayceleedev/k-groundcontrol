// [0.4.1 5.4 T4] specs/features/0.4.1-board-match.md
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
QList<Px4Board> px4Boards(const QByteArray& boardsJson)
{
    QList<Px4Board> result;
    for (const auto value : QJsonDocument::fromJson(boardsJson).array()) {
        const auto object = value.toObject();
        Px4Board board{object.value("boardId").toInt(), object.value("prefix").toString(), {}};
        for (const auto name : object.value("names").toArray())
            if (name.isString()) board.names.append(name.toString());
        result.append(board);
    }
    return result;
}
QStringList px4Prefixes(const QByteArray& boardsJson, int boardId)
{
    if (boardId == kBoardFmuV3) boardId = kBoardFmuV2;
    QStringList result;
    for (const auto& board : px4Boards(boardsJson))
        if (board.boardId == boardId && !board.prefix.isEmpty() && !result.contains(board.prefix)) result.append(board.prefix);
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
QList<OtherBoard> px4OtherBoards(const Px4Release& release, const QList<Px4Board>& boards, int boardId)
{
    QList<OtherBoard> result;
    for (const auto& asset : release.assets) {
        if (!asset.name.endsWith(".px4")) continue;
        for (const auto& board : boards) {
            if (board.prefix.isEmpty() || compatible(boardId, board.boardId) || !asset.name.startsWith(board.prefix + '_')) continue;
            const auto label = asset.name.mid(board.prefix.size() + 1).chopped(4);
            if (label.isEmpty() || label == kBootloaderLabel) continue;
            result.append({board.prefix + " · " + label, board.boardId});
        }
    }
    std::sort(result.begin(), result.end(), [](const OtherBoard& a, const OtherBoard& b) {
        const auto order = a.name.compare(b.name, Qt::CaseInsensitive);
        return order != 0 ? order < 0 : a.boardId < b.boardId;
    });
    return result;
}
}
