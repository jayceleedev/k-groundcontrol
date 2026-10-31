// [0.1.1 T1 T2 T3] specs/features/0.1.1-link-hardening.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/states/LinkLoss.h>
#include <kgc/app/constants/mavlink.h>
namespace kgc::vehicle::states {
bool isLinkLost(qint64 lastMessageMs, qint64 nowMs)
{
    return lastMessageMs >= 0 && nowMs >= lastMessageMs
        && nowMs - lastMessageMs >= kgc::app::constants::kLinkLossTimeout.count();
}
}
