// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <memory>
namespace kgc::vehicle::firmware {
/** 첫 하트비트의 자동조종기 값으로 플러그인을 골라요.
 * 입력: HEARTBEAT의 autopilot 값이에요.
 * 출력: 해당 펌웨어의 데이터와 순수 함수를 제공하는 플러그인이에요.
 * 실패: 없어요.
 */
class FirmwarePluginFactory {
public:
/** 자동조종기 값에 맞는 플러그인을 만들어요.
 * 입력: HEARTBEAT의 autopilot 값이에요.
 * 출력: 호출자가 소유하는 플러그인이며, 알 수 없는 값은 범용 플러그인이에요.
 * 실패: 없어요.
 */
    static std::unique_ptr<FirmwarePlugin> create(std::uint8_t autopilot);
};
}
