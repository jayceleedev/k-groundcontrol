// [0.2.0 T4] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/comm/rawport/ModuleId.h>
#include <QString>
namespace kgc::comm::rawport {
/** 원시 포트를 쓰다 실패한 까닭을 나타내요.
 * 입력: 다른 주인이 있는 포트, 열기 실패, 포트 유실, 읽기 시간 초과 상황이에요.
 * 출력: 각 상황에 대응하는 PortError 값이에요.
 * 실패: 없어요.
 */
enum class PortError : std::uint16_t { Busy = 1, OpenFailed, Lost, Timeout };
/** 포트 오류를 공통 코드로 바꿔요.
 * 입력: 변환할 PortError인 error예요.
 * 출력: comm/rawport 모듈 번호와 오류 번호를 담은 ErrorCode예요.
 * 실패: 없어요.
 */
inline kgc::app::errors::ErrorCode make(PortError error)
{
    return {kModuleId, static_cast<std::uint16_t>(error)};
}
/** 포트 오류를 화면 문구로 바꿔요.
 * 입력: 표시할 공통 오류 코드 error예요.
 * 출력: 현재 언어의 문구이며, 다른 모듈이나 모르는 번호는 빈 문자열이에요.
 * 실패: 없어요.
 */
QString errorText(kgc::app::errors::ErrorCode error);
}
