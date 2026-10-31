// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/outbound/ModuleId.h>
#include <QCoreApplication>
namespace kgc::vehicle::outbound {
/** 송신할 링크가 없는 경우를 나타내요.
 * 입력: 열린 대상 링크를 찾거나 송신을 예약하지 못한 상황이에요.
 * 출력: OutboundError::NoLink 값이에요.
 * 실패: 없어요.
 */
enum class OutboundError : std::uint16_t { NoLink = 1 };
/** 송신 오류를 공통 코드로 바꿔요.
 * 입력: 변환할 OutboundError인 error예요.
 * 출력: vehicle/outbound 모듈 번호와 오류 번호를 담은 ErrorCode예요.
 * 실패: 없어요.
 */
inline kgc::app::errors::ErrorCode make(OutboundError error) { return {kModuleId, static_cast<std::uint16_t>(error)}; }
/** 송신 오류를 화면 문구로 바꿔요.
 * 입력: 표시할 공통 오류 코드 error예요.
 * 출력: NoLink의 현재 언어 문구이며, 다른 코드는 빈 문자열이에요.
 * 실패: 없어요.
 */
inline QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error == make(OutboundError::NoLink))
        return QCoreApplication::translate("Outbound", QT_TRANSLATE_NOOP("Outbound", "보낼 수 있는 연결이 없어요."));
    return {};
}
}
