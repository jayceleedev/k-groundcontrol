// [0.6.0 5.5] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
namespace kgc::vehicle::firmware {
/** 시동·시동 끄기 단계를 만들어요(세 플러그인 공통).
 * 입력: 시동이면 참인 arm, param2에 넣을 force(비상 정지는 21196)예요.
 * 출력: Long COMPONENT_ARM_DISARM, param1 = arm ? 1 : 0, param2 = force, confirmArmed = arm인 단계예요.
 * 실패: 없어요.
 */
CommandStep armStep(bool arm, float force);
/** 모드 바꾸기 단계를 만들어요.
 * 입력: DO_SET_MODE의 param2·param3(사용자 모드 칸)과 확인할 모드 목록 confirm이에요.
 * 출력: Long DO_SET_MODE, param1 = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, confirmModes = confirm인 단계예요.
 * 실패: 없어요.
 */
CommandStep modeStep(float param2, float param3, QList<std::uint32_t> confirm);
/** 여기로 가기(DO_REPOSITION) 단계를 만들어요.
 * 입력: 좌표 틀 frame, 방향 param4, 위도 x·경도 y(도 × 1e7), 고도 z, 확인할 유도 모드 confirm이에요.
 * 출력: Int DO_REPOSITION, param1 -1(기본 속도), param2 MAV_DO_REPOSITION_FLAGS_CHANGE_MODE, param3 0, confirmModes = {confirm}인 단계예요.
 * 실패: 없어요.
 */
CommandStep repositionStep(std::uint8_t frame, float param4, std::int32_t x, std::int32_t y, float z, std::uint32_t confirm);
/** PX4 사용자 모드 값을 만들어요.
 * 입력: 주 모드 main과 보조 모드 sub예요.
 * 출력: (main << 16) | (sub << 24)예요.
 * 실패: 없어요.
 */
std::uint32_t px4Mode(std::uint32_t main, std::uint32_t sub);
}
