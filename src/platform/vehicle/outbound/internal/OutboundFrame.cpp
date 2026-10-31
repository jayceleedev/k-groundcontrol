// [0.1.1 T7] specs/features/0.1.1-link-hardening.md
#include "OutboundFrame.h"
#include <kgc/comm/links/LinkManager.h>
#include <kgc/app/constants/mavlink.h>
#include <array>
namespace kgc::vehicle::outbound {
bool packAndWrite(kgc::comm::links::LinkManager& links, int linkId, const VehicleSender::PackFn& pack)
{
    if (linkId < 0 || links.linkId() != linkId) return false;
    const auto channel = links.channel();
    mavlink_get_channel_status(channel)->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
    mavlink_message_t message{};
    pack(kgc::app::constants::kGcsSysid, kgc::app::constants::kGcsCompid, channel, message);
    std::array<std::uint8_t, MAVLINK_MAX_PACKET_LEN> bytes{};
    const auto length = mavlink_msg_to_send_buffer(bytes.data(), &message);
    return links.writeBytes(linkId, QByteArray(reinterpret_cast<const char*>(bytes.data()), length));
}
}
