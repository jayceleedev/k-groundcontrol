// [0.2.0 T9] specs/features/0.2.0-fcflash.md
#include "ArduPilotManifest.h"
namespace kgc::fcFlash {
QList<ManifestEntry> scanManifest(const QByteArray& bytes, int boardId)
{
    // WORKER-FILL: spec §6.5 — firmware 배열을 문자열을 살피며 항목 단위로 자르고 맞는 항목만 QJsonDocument로 해석해요.
    Q_UNUSED(bytes);
    Q_UNUSED(boardId);
    return {};
}
QList<ManifestVersion> manifestVersions(const QList<ManifestEntry>& entries, ArduPilotVehicle vehicle)
{
    // WORKER-FILL: spec §6.5 — OFFICIAL 다음 STABLE-x.y.z를 수 비교로 최신 순으로 줄 세워요.
    Q_UNUSED(entries);
    Q_UNUSED(vehicle);
    return {};
}
QList<ManifestEntry> manifestCandidates(const QList<ManifestEntry>& entries, ArduPilotVehicle vehicle, const QString& buildType)
{
    // WORKER-FILL: spec §6.5 — 기체와 빌드 종류가 같은 항목을 골라요.
    Q_UNUSED(entries);
    Q_UNUSED(vehicle);
    Q_UNUSED(buildType);
    return {};
}
}
