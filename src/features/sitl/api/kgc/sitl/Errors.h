// [0.4.0 5.4 7] specs/features/0.4.0-sitl.md
#pragma once
#include <kgc/sitl/ModuleId.h>
#include <QString>
namespace kgc::sitl {
/** 시뮬레이터를 받거나 띄우거나 잇지 못한 까닭을 나타내요.
 * 입력: 스펙 §6의 실패 상황이에요.
 * 출력: 각 상황에 대응하는 SitlError 값이에요.
 * 실패: 없어요.
 */
enum class SitlError : std::uint16_t {
    LinkBusy = 1, LinkFailed, DownloadFailed, StartFailed, Crashed, BadHome, Busy
};
/** 시뮬레이션 오류를 공통 코드로 바꿔요.
 * 입력: 변환할 SitlError인 error예요.
 * 출력: sitl 모듈 번호와 오류 번호를 담은 ErrorCode예요.
 * 실패: 없어요.
 */
inline kgc::app::errors::ErrorCode make(SitlError error)
{
    return {kModuleId, static_cast<std::uint16_t>(error)};
}
/** 시뮬레이션 오류를 화면 문구로 바꿔요.
 * 입력: 표시할 공통 오류 코드 error예요.
 * 출력: 현재 언어의 문구(스펙 §7)이며, 다른 모듈의 코드는 빈 문자열이에요.
 * 실패: 없어요.
 */
QString errorText(kgc::app::errors::ErrorCode error);
}
