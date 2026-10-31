// [0.2.0 T12] specs/features/0.2.0-fcflash.md
#include "Candidates.h"
namespace kgc::fcFlash {
CandidatePick pickCandidate(int count)
{
    // WORKER-FILL: spec §6.5 — 0은 None, 1은 Auto, 그보다 많으면 Choose예요.
    static_cast<void>(count);
    return CandidatePick::None;
}
}
