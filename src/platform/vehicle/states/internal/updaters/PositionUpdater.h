// [0.5.0 S3 T5] specs/features/0.5.0-map.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/states/PositionBundle.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
namespace kgc::vehicle::states {
/** GLOBAL_POSITION_INT를 Position 묶음에 반영해요.
 * 입력: 완성 메시지 message와 지금 묶음 current예요.
 * 출력: 위치·고도 넷·속도·상승률·헤딩 일곱 필드를 덮어쓴 묶음이에요. 헤딩은 늘 새 값(모르면 NaN)이고 홈 세 필드는 그대로예요.
 * 실패: 없어요.
 */
PositionBundle updatePosition(const mavlink_message_t& message, PositionBundle current);
}
