// [0.4.1 5.4 T3] specs/features/0.4.1-board-match.md
// [0.2.0 T9] specs/features/0.2.0-fcflash.md
#include "ArduPilotManifest.h"
#include "Constants.h"
#include "image/FirmwareImage.h"
#include <QByteArrayView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSet>
#include <QVersionNumber>
#include <algorithm>
#include <optional>
namespace kgc::fcFlash {
namespace {
/** 항목의 맨 위 필드 중 보드·기체·빌드 종류를 가르는 값만 모아요. */
struct ItemFields {
    QByteArrayView format;
    QByteArrayView vehicleType;
    QByteArrayView mavType;
    QByteArrayView buildType;
    QByteArrayView platform;
    int boardId = -1;
};
qsizetype stringEnd(QByteArrayView bytes, qsizetype quote)
{
    for (qsizetype i = quote + 1; i < bytes.size(); ++i) {
        if (bytes[i] == '\\') ++i;
        else if (bytes[i] == '"') return i + 1;
    }
    return bytes.size();
}
qsizetype skipSpace(QByteArrayView bytes, qsizetype i)
{
    while (i < bytes.size() && (bytes[i] == ' ' || bytes[i] == '\n' || bytes[i] == '\r' || bytes[i] == '\t')) ++i;
    return i;
}
QByteArrayView* fieldFor(ItemFields& fields, QByteArrayView key)
{
    if (key == "format") return &fields.format;
    if (key == "vehicletype") return &fields.vehicleType;
    if (key == "mav-type") return &fields.mavType;
    if (key == "mav-firmware-version-type") return &fields.buildType;
    if (key == "platform") return &fields.platform;
    return nullptr;
}
int numberAt(QByteArrayView bytes, qsizetype i)
{
    int value = -1;
    for (; i < bytes.size() && bytes[i] >= '0' && bytes[i] <= '9'; ++i) value = (value < 0 ? 0 : value * 10) + (bytes[i] - '0');
    return value;
}
// 다른 보드 항목은 수만 개라서 객체를 만들지 않고 깊이 1의 키만 한 번 훑어 읽어요.
ItemFields itemFields(QByteArrayView item)
{
    ItemFields fields;
    int depth = 0;
    for (qsizetype i = 0; i < item.size(); ++i) {
        const char c = item[i];
        if (c == '{' || c == '[') { ++depth; continue; }
        if (c == '}' || c == ']') { --depth; continue; }
        if (c != '"') continue;
        const auto end = stringEnd(item, i);
        const auto colon = skipSpace(item, end);
        if (depth != 1 || colon >= item.size() || item[colon] != ':') { i = end - 1; continue; }
        const auto key = item.sliced(i + 1, end - i - 2);
        const auto value = skipSpace(item, colon + 1);
        i = value - 1;
        if (value >= item.size()) break;
        if (key == "board_id") { fields.boardId = numberAt(item, value); continue; }
        auto* target = fieldFor(fields, key);
        if (!target || item[value] != '"') continue;
        const auto valueEnd = stringEnd(item, value);
        *target = item.sliced(value + 1, std::max<qsizetype>(0, valueEnd - value - 2));
        i = valueEnd - 1;
    }
    return fields;
}
bool buildTypeAllowed(const QString& buildType)
{
    static const QRegularExpression stable(QStringLiteral(R"(^STABLE-\d+\.\d+\.\d+$)"));
    return buildType == "OFFICIAL" || stable.match(buildType).hasMatch();
}
std::optional<ArduPilotVehicle> vehicleOf(const QString& vehicleType, const QString& mavType)
{
    if (vehicleType == "Copter" || vehicleType == "Heli")
        return mavType == "HELICOPTER" ? ArduPilotVehicle::Heli : ArduPilotVehicle::Copter;
    if (vehicleType == "Plane") return ArduPilotVehicle::Plane;
    if (vehicleType == "Rover") return ArduPilotVehicle::Rover;
    if (vehicleType == "Sub") return ArduPilotVehicle::Sub;
    return {};
}
int versionPart(const QJsonValue& value)
{
    return value.isString() ? value.toString().toInt() : value.toInt();
}
std::optional<ManifestEntry> manifestEntry(const QByteArray& bytes, int boardId)
{
    const auto object = QJsonDocument::fromJson(bytes).object();
    if (object.value("format").toString() != "apj" || object.value("board_id").toInt(-1) != boardId) return {};
    ManifestEntry entry;
    entry.buildType = object.value("mav-firmware-version-type").toString();
    if (!buildTypeAllowed(entry.buildType)) return {};
    const auto vehicle = vehicleOf(object.value("vehicletype").toString(), object.value("mav-type").toString());
    if (!vehicle) return {};
    entry.vehicle = *vehicle;
    entry.version = QStringLiteral("%1.%2.%3")
        .arg(versionPart(object.value("mav-firmware-version-major")))
        .arg(versionPart(object.value("mav-firmware-version-minor")))
        .arg(versionPart(object.value("mav-firmware-version-patch")));
    entry.platform = object.value("platform").toString();
    entry.url = object.value("url").toString();
    const auto names = object.value("bootloader_str");
    if (names.isString()) entry.bootloaderNames.append(names.toString());
    for (const auto name : names.toArray())
        if (name.isString()) entry.bootloaderNames.append(name.toString());
    return entry;
}
std::optional<ManifestBoard> manifestBoard(const ItemFields& fields)
{
    if (fields.format != "apj" || fields.boardId < 0) return {};
    ManifestBoard board;
    board.buildType = QString::fromUtf8(fields.buildType);
    if (!buildTypeAllowed(board.buildType)) return {};
    const auto vehicle = vehicleOf(QString::fromUtf8(fields.vehicleType), QString::fromUtf8(fields.mavType));
    if (!vehicle) return {};
    board.vehicle = *vehicle;
    board.platform = QString::fromUtf8(fields.platform);
    board.boardId = fields.boardId;
    return board;
}
}
ManifestScan scanManifest(const QByteArray& bytes, int boardId)
{
    if (boardId == kBoardFmuV3) boardId = kBoardFmuV2;
    ManifestScan result;
    const auto key = bytes.indexOf("\"firmware\"");
    if (key < 0) return result;
    const auto array = bytes.indexOf('[', key);
    if (array < 0) return result;
    QSet<QString> seen;
    bool quoted = false;
    bool escaped = false;
    int depth = 0;
    qsizetype start = -1;
    for (qsizetype i = array + 1; i < bytes.size(); ++i) {
        const char c = bytes[i];
        if (quoted) {
            if (escaped) escaped = false;
            else if (c == '\\') escaped = true;
            else if (c == '"') quoted = false;
            continue;
        }
        if (c == '"') quoted = true;
        else if (c == '{') { if (depth++ == 0) start = i; }
        else if (c == '}' && depth > 0 && --depth == 0) {
            const auto item = QByteArrayView(bytes).sliced(start, i - start + 1);
            const auto fields = itemFields(item);
            if (fields.boardId == boardId) {
                const auto entry = manifestEntry(item.toByteArray(), boardId);
                if (entry) result.entries.append(*entry);
            } else if (fields.boardId >= 0 && !compatible(boardId, fields.boardId)) {
                const auto board = manifestBoard(fields);
                if (!board) continue;
                const auto id = QStringLiteral("%1\n%2\n%3\n%4").arg(board->platform).arg(board->boardId)
                    .arg(static_cast<int>(board->vehicle)).arg(board->buildType);
                if (seen.contains(id)) continue;
                seen.insert(id);
                result.boards.append(*board);
            }
        } else if (c == ']' && depth == 0) break;
    }
    return result;
}
QList<ManifestVersion> manifestVersions(const QList<ManifestEntry>& entries, ArduPilotVehicle vehicle)
{
    QList<ManifestVersion> result;
    for (const auto& entry : entries) {
        if (entry.vehicle != vehicle) continue;
        if (std::none_of(result.cbegin(), result.cend(), [&](const auto& value) { return value.buildType == entry.buildType; }))
            result.append({entry.buildType, entry.version});
    }
    std::stable_sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
        if ((a.buildType == "OFFICIAL") != (b.buildType == "OFFICIAL")) return a.buildType == "OFFICIAL";
        return QVersionNumber::fromString(a.version) > QVersionNumber::fromString(b.version);
    });
    return result;
}
QList<ManifestEntry> manifestCandidates(const QList<ManifestEntry>& entries, ArduPilotVehicle vehicle, const QString& buildType)
{
    QList<ManifestEntry> result;
    for (const auto& entry : entries)
        if (entry.vehicle == vehicle && entry.buildType == buildType) result.append(entry);
    return result;
}
QList<OtherBoard> manifestOtherBoards(const ManifestScan& scan, ArduPilotVehicle vehicle, const QString& buildType)
{
    QSet<QString> candidates;
    for (const auto& entry : manifestCandidates(scan.entries, vehicle, buildType)) candidates.insert(entry.platform);
    QList<OtherBoard> result;
    for (const auto& board : scan.boards)
        if (board.vehicle == vehicle && board.buildType == buildType && !candidates.contains(board.platform))
            result.append({board.platform, board.boardId});
    std::sort(result.begin(), result.end(), [](const OtherBoard& a, const OtherBoard& b) {
        const auto order = a.name.compare(b.name, Qt::CaseInsensitive);
        return order != 0 ? order < 0 : a.boardId < b.boardId;
    });
    return result;
}
}
