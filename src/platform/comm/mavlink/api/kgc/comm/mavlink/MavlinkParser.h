// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/comm/mavlink/Mavlink.h>
namespace kgc::comm::mavlink {
/** 링크마다 독립된 수신 상태로 바이트를 프레임으로 해석해요.
 * 입력: 순서대로 수신한 바이트예요.
 * 출력: 정상 프레임의 메시지와 버전 정보를 제공해요.
 * 실패: 손상된 프레임은 완성 메시지로 내놓지 않아요.
 */
class MavlinkParser {
public:
/** 한 바이트를 받아 완성된 프레임만 돌려줘요.
 * 입력: 수신 byte와 결과를 받을 message·isV1 참조예요.
 * 출력: 정상 프레임이 완성되면 참이며, message와 isV1이 유효해요.
 * 실패: CRC 오류와 불완전 프레임은 거짓이며 출력 참조를 사용하면 안 돼요.
 */
    bool parse(std::uint8_t byte, mavlink_message_t& message, bool& isV1);
private:
    mavlink_message_t buffer_{};
    mavlink_status_t status_{};
};
/** v1에서 하트비트만 사용할지 판정해요.
 * 입력: v1 여부인 isV1과 메시지 번호 messageId예요.
 * 출력: v1의 하트비트 이외 메시지면 참이고, 나머지는 거짓이에요.
 * 실패: 없어요.
 */
bool dropV1NonHeartbeat(bool isV1, std::uint32_t messageId);
}
