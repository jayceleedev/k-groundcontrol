// [0.1.1 T7 T8 T9 T10] specs/features/0.1.1-link-hardening.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/outbound/VehicleSender.h>
#include <kgc/vehicle/outbound/Errors.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <kgc/comm/links/LinkManager.h>
#include <kgc/app/scheduler/Scheduler.h>
#include "Constants.h"
#include "OutboundFrame.h"
#include <QThread>
namespace kgc::vehicle::outbound {
VehicleSender::VehicleSender(kgc::comm::links::LinkManager& links, kgc::app::scheduler::Scheduler& scheduler,
                             int sysid, int linkId, const kgc::vehicle::firmware::FirmwarePlugin& plugin, QObject* parent)
    : QObject(parent), links_(&links), plugin_(plugin), sysid_(sysid), linkId_(linkId)
{
    scheduler.registerTask(QString("streamRequests.%1").arg(sysid), kStreamRequestPeriod,
                           this, this, [this] { sendStreamRequestRound(); });
    sendStreamRequests();
}
kgc::app::errors::Result<void> VehicleSender::send(OutboundKind, PackFn pack)
{
    Q_ASSERT(thread() == QThread::currentThread());
    if (!links_ || linkId_ < 0 || links_->linkId() != linkId_) return std::unexpected(make(OutboundError::NoLink));
    if (!packAndWrite(*links_, linkId_, pack))
        return std::unexpected(make(OutboundError::NoLink));
    return {};
}
void VehicleSender::sendStreamRequests() { remainingRounds_ = kStreamRequestRounds; }
void VehicleSender::sendStreamRequestRound()
{
    if (remainingRounds_ == 0) return;
    for (const auto request : plugin_.streamRequests()) {
        send(OutboundKind::Other, [this, request](std::uint8_t sysid, std::uint8_t compid,
                                                std::uint8_t channel, mavlink_message_t& message) {
            mavlink_msg_request_data_stream_pack_chan(sysid, compid, channel, &message,
                sysid_, MAV_COMP_ID_AUTOPILOT1, request.streamId, request.rateHz, 1);
        });
    }
    --remainingRounds_;
}
}
