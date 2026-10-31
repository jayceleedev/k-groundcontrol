// [0.2.0 T9] specs/features/0.2.0-fcflash.md
#include "ArduPilotManifest.h"
#include "Constants.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QVersionNumber>
#include <algorithm>
#include <optional>
namespace kgc::fcFlash {
namespace {
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
    static const QRegularExpression stable(QStringLiteral(R"(^STABLE-\d+\.\d+\.\d+$)"));
    if (entry.buildType != "OFFICIAL" && !stable.match(entry.buildType).hasMatch()) return {};
    const auto vehicle = object.value("vehicletype").toString();
    if (vehicle == "Copter" || vehicle == "Heli")
        entry.vehicle = object.value("mav-type").toString() == "HELICOPTER" ? ArduPilotVehicle::Heli : ArduPilotVehicle::Copter;
    else if (vehicle == "Plane") entry.vehicle = ArduPilotVehicle::Plane;
    else if (vehicle == "Rover") entry.vehicle = ArduPilotVehicle::Rover;
    else if (vehicle == "Sub") entry.vehicle = ArduPilotVehicle::Sub;
    else return {};
    entry.version = QStringLiteral("%1.%2.%3")
        .arg(versionPart(object.value("mav-firmware-version-major")))
        .arg(versionPart(object.value("mav-firmware-version-minor")))
        .arg(versionPart(object.value("mav-firmware-version-patch")));
    entry.platform = object.value("platform").toString();
    entry.url = object.value("url").toString();
    return entry;
}
}
QList<ManifestEntry> scanManifest(const QByteArray& bytes, int boardId)
{
    if (boardId == kBoardFmuV3) boardId = kBoardFmuV2;
    QList<ManifestEntry> result;
    const auto key = bytes.indexOf("\"firmware\"");
    if (key < 0) return result;
    const auto array = bytes.indexOf('[', key);
    if (array < 0) return result;
    const QRegularExpression boardField(QStringLiteral("\"board_id\"\\s*:\\s*%1\\s*[,}]").arg(boardId));
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
            const auto item = bytes.mid(start, i - start + 1);
            // 보드가 다른 대부분의 항목은 객체를 만들지 않아 큰 목록의 메모리 사용을 줄여요.
            if (boardField.match(QString::fromUtf8(item)).hasMatch()) {
                const auto entry = manifestEntry(item, boardId);
                if (entry) result.append(*entry);
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
}
