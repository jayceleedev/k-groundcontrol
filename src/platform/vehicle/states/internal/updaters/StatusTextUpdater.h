// [0.6.0 5.4 T2] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/vehicle/states/VehicleMessage.h>
#include <kgc/comm/mavlink/Mavlink.h>
namespace kgc::vehicle::states {
/** STATUSTEXT 하나를 기체 메시지로 풀어요.
 * 입력: 완성 메시지 message예요.
 * 출력: severity와, 끝 NUL까지(없으면 50바이트 전부) UTF-8로 푼 text예요. 나뉜 긴 글(id·chunk_seq)은 조각 하나를 그대로 내요.
 * 실패: 없어요.
 */
VehicleMessage statusTextOf(const mavlink_message_t& message);
}
