// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/mavlink/MavlinkParser.h>
namespace kgc::comm::mavlink {
bool MavlinkParser::parse(std::uint8_t byte, mavlink_message_t& message, bool& isV1)
{
    mavlink_status_t result{};
    const auto framing = mavlink_frame_char_buffer(&buffer_, &status_, byte, &message, &result);
    if (framing == MAVLINK_FRAMING_BAD_CRC || framing == MAVLINK_FRAMING_BAD_SIGNATURE) {
        // 잘린 프레임의 끝으로 읽은 바이트가 다음 시작 표식이면 새 프레임에 돌려줘요.
        status_.parse_state = MAVLINK_PARSE_STATE_IDLE;
        status_.msg_received = MAVLINK_FRAMING_INCOMPLETE;
        if (byte == MAVLINK_STX || byte == MAVLINK_STX_MAVLINK1)
            mavlink_frame_char_buffer(&buffer_, &status_, byte, &message, &result);
        return false;
    }
    if (framing != MAVLINK_FRAMING_OK) return false;
    isV1 = message.magic == MAVLINK_STX_MAVLINK1;
    return true;
}
bool dropV1NonHeartbeat(bool isV1, std::uint32_t messageId)
{
    return isV1 && messageId != MAVLINK_MSG_ID_HEARTBEAT;
}
}
