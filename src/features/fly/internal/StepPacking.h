// [0.6.0 5.11.3 T9] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/fly/Errors.h>
#include <kgc/vehicle/command/CommandQueue.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <kgc/vehicle/outbound/OutboundKind.h>
namespace kgc::fly {
/** 플러그인의 Long 단계를 큐에 넣을 COMMAND_LONG으로 옮겨요.
 * 입력: 명령 단계 step이에요.
 * 출력: 명령 번호와 param1~7을 그대로 옮기고 targetCompid 1인 CommandLong이에요.
 * 실패: 없어요.
 */
kgc::vehicle::command::CommandLong toCommandLong(const kgc::vehicle::firmware::CommandStep& step);
/** 플러그인의 Int 단계를 큐에 넣을 COMMAND_INT로 옮겨요.
 * 입력: 명령 단계 step이에요.
 * 출력: 명령 번호·frame·param1~4·x·y·z를 그대로 옮기고 targetCompid 1인 CommandInt예요.
 * 실패: 없어요.
 */
kgc::vehicle::command::CommandInt toCommandInt(const kgc::vehicle::firmware::CommandStep& step);
/** 단계의 송신 종류를 정해요(작업 잠금이 볼 값).
 * 입력: 명령 단계 step이에요.
 * 출력: COMPONENT_ARM_DISARM이면 Arm, 그 밖은 Motion이에요.
 * 실패: 없어요.
 */
kgc::vehicle::outbound::OutboundKind outboundKindOf(const kgc::vehicle::firmware::CommandStep& step);
/** 기체가 돌려준 ACK 결과를 비행 오류로 바꿔요.
 * 입력: ACCEPTED가 아닌 MAV_RESULT result예요.
 * 출력: DENIED → Denied, UNSUPPORTED → Unsupported, TEMPORARILY_REJECTED → TemporarilyRejected, 그 밖 → Failed예요.
 * 실패: 없어요.
 */
FlyError errorOf(MAV_RESULT result);
/** ACK 없이 끝난 까닭을 비행 오류로 바꿔요.
 * 입력: 큐의 실패 failure예요.
 * 출력: Timeout → NoAck, Gone → Gone, NoLink → NoLink, Preempted → Preempted예요.
 * 실패: 없어요.
 */
FlyError errorOf(kgc::vehicle::command::CommandFailure failure);
}
