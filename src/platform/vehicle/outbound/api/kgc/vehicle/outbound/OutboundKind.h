// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
namespace kgc::vehicle::outbound {
/** 송신 목적을 표시해 이후 안전 정책을 적용할 수 있게 해요.
 * 입력: 시동·이동·명령·설정·미션·그 밖의 송신 목적이에요.
 * 출력: Arm·Motion·Command·Param·Mission·Other 중 하나이며, 이번 버전은 Other만 사용해요.
 * 실패: 없어요.
 */
enum class OutboundKind { Arm, Motion, Command, Param, Mission, Other };
}
