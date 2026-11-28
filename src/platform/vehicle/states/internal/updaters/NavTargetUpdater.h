// [0.6.0 5.4 T2] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/vehicle/states/NavTargetBundle.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <QtGlobal>
#include <chrono>
namespace kgc::vehicle::states {
/** POSITION_TARGET_GLOBAL_INT를 NavTarget 묶음의 목표 셋에 반영해요.
 * 입력: 완성 메시지 message와 지금 묶음 current예요.
 * 출력: type_mask에 X·Y 무시 비트가 있거나 lat_int·lon_int가 둘 다 0이면 위도·경도 NaN, 아니면 1e-7도를 도로 바꾼 값이에요.
 *       altM은 alt 그대로, targetCount는 앞 값 + 1이에요. 선회 세 필드는 그대로예요.
 * 실패: 없어요.
 */
NavTargetBundle updateNavTarget(const mavlink_message_t& message, NavTargetBundle current);
/** ORBIT_EXECUTION_STATUS를 NavTarget 묶음의 선회 셋에 반영해요.
 * 입력: 완성 메시지 message와 지금 묶음 current예요.
 * 출력: radius가 0이면 orbitRadiusM NaN, 아니면 그 값(부호 있음)이에요. frame이 전역 프레임이면 x·y를 1e-7도에서 도로, 아니면 NaN이에요(PX4는 frame 0을 보내요).
 *       목표 세 필드와 targetCount는 그대로예요.
 * 실패: 없어요.
 */
NavTargetBundle updateOrbit(const mavlink_message_t& message, NavTargetBundle current);
/** 마지막으로 받은 때부터 한도보다 오래 지났는지 판정해요.
 * 입력: 같은 단조 시계로 잰 받은 때 receivedMs와 지금 nowMs, 한도 limit예요.
 * 출력: nowMs - receivedMs가 limit보다 크면 참이에요(같으면 거짓).
 * 실패: 없어요.
 */
bool isStale(qint64 receivedMs, qint64 nowMs, std::chrono::milliseconds limit);
}
