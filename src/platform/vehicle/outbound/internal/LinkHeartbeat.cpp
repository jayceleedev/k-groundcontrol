// [0.1.1 T8 T9] specs/features/0.1.1-link-hardening.md
#include <kgc/vehicle/outbound/LinkHeartbeat.h>
#include <kgc/comm/links/LinkManager.h>
#include <kgc/app/scheduler/Scheduler.h>
#include <kgc/app/constants/mavlink.h>
#include "OutboundFrame.h"
namespace kgc::vehicle::outbound {
LinkHeartbeat::LinkHeartbeat(kgc::comm::links::LinkManager& links, kgc::app::scheduler::Scheduler& scheduler,
                             QObject* parent)
    : QObject(parent), links_(&links)
{
    connect(&links, &kgc::comm::links::LinkManager::linkOpened, this, [this] { send(); });
    scheduler.registerTask("gcsHeartbeat", kgc::app::constants::kGcsHeartbeatPeriod,
                           this, this, [this] { send(); });
}
void LinkHeartbeat::send()
{
    if (!links_) return;
    packAndWrite(*links_, links_->linkId(), [](std::uint8_t sysid, std::uint8_t compid,
                                            std::uint8_t channel, mavlink_message_t& message) {
        mavlink_msg_heartbeat_pack_chan(sysid, compid, channel, &message, MAV_TYPE_GCS,
                                      MAV_AUTOPILOT_INVALID, 0, 0, MAV_STATE_ACTIVE);
    });
}
}
