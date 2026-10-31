// [0.2.0 T10] specs/features/0.2.0-fcflash.md
#include "Px4Releases.h"
namespace kgc::fcFlash {
QList<Px4Release> parsePx4Releases(const QByteArray& json)
{
    // WORKER-FILL: spec §6.5 — draft·prerelease를 빼고 최신 순으로 줄 세워요. version은 versionInText(tag)예요.
    Q_UNUSED(json);
    return {};
}
QStringList px4Prefixes(const QByteArray& boardsJson, int boardId)
{
    // WORKER-FILL: spec §6.5 — [{"boardId":…,"prefix":…}]에서 boardId가 같은 prefix를 모아요.
    Q_UNUSED(boardsJson);
    Q_UNUSED(boardId);
    return {};
}
QList<Px4Candidate> px4Candidates(const Px4Release& release, const QStringList& prefixes)
{
    // WORKER-FILL: spec §6.5 — 자산 이름이 <prefix>_<꼬리표>.px4인 것을 모으고 꼬리표 bootloader는 빼요.
    Q_UNUSED(release);
    Q_UNUSED(prefixes);
    return {};
}
}
