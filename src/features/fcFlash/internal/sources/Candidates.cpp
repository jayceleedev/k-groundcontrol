// [0.2.0 T12] specs/features/0.2.0-fcflash.md
#include "Candidates.h"
namespace kgc::fcFlash {
CandidatePick pickCandidate(int count)
{
    return count <= 0 ? CandidatePick::None : count == 1 ? CandidatePick::Auto : CandidatePick::Choose;
}
}
