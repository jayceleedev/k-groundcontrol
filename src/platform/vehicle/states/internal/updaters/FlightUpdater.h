// [0.6.0 5.4 T2] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/vehicle/states/FlightBundle.h>
#include <kgc/comm/mavlink/Mavlink.h>
namespace kgc::vehicle::states {
/** EXTENDED_SYS_STATE를 Flight 묶음으로 옮겨요.
 * 입력: 완성 메시지 message예요.
 * 출력: landed_state·vtol_state를 MAVLink 값 그대로 담은 묶음이에요.
 * 실패: 없어요.
 */
FlightBundle updateFlight(const mavlink_message_t& message);
}
