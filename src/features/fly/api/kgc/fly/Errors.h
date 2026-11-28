// [0.6.0 5.11.2 6] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/fly/ModuleId.h>
#include <QString>
#include <cstdint>
namespace kgc::fly {
/** 비행 명령이 보내지지 않았거나 실패한 까닭을 나타내요.
 * 입력: 스펙 §5.11의 실패 상황이에요(기체 응답, 확인 시간 초과, 화면 쪽 검사).
 * 출력: 각 상황에 대응하는 FlyError 값이에요. 화면 글은 errorText가 만들어요.
 * 실패: 없어요.
 */
enum class FlyError : std::uint16_t {
    NoLink = 1, NoAck, Denied, Unsupported, TemporarilyRejected, Failed, ModeNotChanged, ArmNotChanged,
    TargetRejected, Busy, NotReady, HomeUnknown, PositionUnknown, TooFar, AltitudeOutOfRange, Preempted,
    PolicyDenied, Gone, TargetWithoutMode
};
/** 비행 오류를 공통 코드로 바꿔요.
 * 입력: 바꿀 FlyError인 error예요.
 * 출력: fly 모듈 번호(114)와 오류 번호를 담은 ErrorCode예요.
 * 실패: 없어요.
 */
inline kgc::app::errors::ErrorCode make(FlyError error)
{
    return {kModuleId, static_cast<std::uint16_t>(error)};
}
/** 비행 오류를 화면 글로 바꿔요(번역 문맥 "Fly").
 * 입력: 공통 오류 코드 error예요.
 * 출력: 지금 언어의 글이에요. 다른 모듈의 코드는 빈 글이에요.
 * 실패: 없어요.
 */
QString errorText(kgc::app::errors::ErrorCode error);
}
