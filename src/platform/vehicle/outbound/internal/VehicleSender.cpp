// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/vehicle/outbound/VehicleSender.h>
#include <kgc/vehicle/outbound/Errors.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <kgc/comm/links/LinkManager.h>
#include <kgc/app/scheduler/Scheduler.h>
#include <kgc/app/constants/mavlink.h>
#include <QThread>
#include <array>
namespace kgc::vehicle::outbound {
VehicleSender::VehicleSender(kgc::comm::links::LinkManager& links, kgc::app::scheduler::Scheduler& scheduler,
                             int sysid, int linkId, const kgc::vehicle::firmware::FirmwarePlugin& plugin, QObject* parent)
    : QObject(parent), links_(&links), plugin_(plugin), sysid_(sysid), linkId_(linkId)
{
    scheduler.registerTask(QString("gcsHeartbeat.%1").arg(sysid), kgc::app::constants::kGcsHeartbeatPeriod,
                           this, this, [this] { sendHeartbeat(); });
    sendStreamRequests();
}
kgc::app::errors::Result<void> VehicleSender::send(OutboundKind, PackFn pack)
{
    Q_ASSERT(thread() == QThread::currentThread());
    if (!links_ || linkId_ < 0 || links_->linkId() != linkId_) return std::unexpected(make(OutboundError::NoLink));
    const auto channel = links_->channel();
    mavlink_get_channel_status(channel)->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
    mavlink_message_t message{};
    pack(kgc::app::constants::kGcsSysid, kgc::app::constants::kGcsCompid, channel, message);
    std::array<std::uint8_t, MAVLINK_MAX_PACKET_LEN> bytes{};
    const auto length = mavlink_msg_to_send_buffer(bytes.data(), &message);
    if (!links_->writeBytes(linkId_, QByteArray(reinterpret_cast<const char*>(bytes.data()), length)))
        return std::unexpected(make(OutboundError::NoLink));
    return {};
}
void VehicleSender::sendHeartbeat()
{
    send(OutboundKind::Other, [](std::uint8_t sysid, std::uint8_t compid, std::uint8_t channel, mavlink_message_t& message) {
        mavlink_msg_heartbeat_pack_chan(sysid, compid, channel, &message, MAV_TYPE_GCS,
                                      MAV_AUTOPILOT_INVALID, 0, 0, MAV_STATE_ACTIVE);
    });
}
void VehicleSender::sendStreamRequests()
{
    for (const auto request : plugin_.streamRequests()) {
        send(OutboundKind::Other, [this, request](std::uint8_t sysid, std::uint8_t compid,
                                                std::uint8_t channel, mavlink_message_t& message) {
            mavlink_msg_request_data_stream_pack_chan(sysid, compid, channel, &message,
                sysid_, MAV_COMP_ID_AUTOPILOT1, request.streamId, request.rateHz, 1);
        });
    }
}
}
