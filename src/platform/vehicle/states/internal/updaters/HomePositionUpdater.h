// [0.5.0 S3 T5] specs/features/0.5.0-map.md
#pragma once
#include <kgc/vehicle/states/PositionBundle.h>
#include <kgc/comm/mavlink/Mavlink.h>
namespace kgc::vehicle::states {
/** HOME_POSITION을 Position 묶음의 홈 세 필드에 반영해요.
 * 입력: 완성 메시지 message와 지금 묶음 current예요.
 * 출력: latitude·longitude(1e-7도)를 도로, altitude(mm, AMSL)를 미터로 바꿔 홈 세 필드에만 넣은 묶음이에요. 나머지는 그대로예요.
 * 실패: 없어요.
 */
PositionBundle updateHomePosition(const mavlink_message_t& message, PositionBundle current);
}
