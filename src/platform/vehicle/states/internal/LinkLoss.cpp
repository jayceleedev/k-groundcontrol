// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/states/LinkLoss.h>
#include <kgc/app/constants/mavlink.h>
namespace kgc::vehicle::states {
bool isLinkLost(qint64 lastHeartbeatMs, qint64 nowMs)
{
    return lastHeartbeatMs >= 0 && nowMs >= lastHeartbeatMs
        && nowMs - lastHeartbeatMs >= kgc::app::constants::kLinkLossTimeout.count();
}
}
