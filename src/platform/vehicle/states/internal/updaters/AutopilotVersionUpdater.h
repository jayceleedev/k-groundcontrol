// [0.2.0 T11] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/vehicle/states/CapabilitiesBundle.h>
#include <kgc/comm/mavlink/Mavlink.h>
namespace kgc::vehicle::states {
/** AUTOPILOT_VERSION을 Capabilities 묶음에 반영해요.
 * 입력: 완성 메시지 message와 지금 묶음 current예요.
 * 출력: flight_sw_version의 윗 세 바이트를 major.minor.patch로 풀어 firmwareVersion에 넣은 묶음이에요. 나머지 값은 그대로예요.
 * 실패: 없어요.
 */
CapabilitiesBundle updateAutopilotVersion(const mavlink_message_t& message, CapabilitiesBundle current);
}
