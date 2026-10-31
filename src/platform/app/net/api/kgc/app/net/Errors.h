// [0.2.0 T3] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/net/ModuleId.h>
#include <QString>
namespace kgc::app::net {
/** HTTP 요청이 실패한 까닭을 나타내요.
 * 입력: https가 아닌 주소, 허용되지 않은 리다이렉트, 네트워크·시간 초과, 파일 쓰기, 취소, 깨진 gzip 상황이에요.
 * 출력: 각 상황에 대응하는 HttpError 값이에요.
 * 실패: 없어요.
 */
enum class HttpError : std::uint16_t {
    NotHttps = 1, RedirectRefused, NetworkFailed, TimedOut, WriteFailed, Aborted, BadGzip
};
/** HTTP 오류를 공통 코드로 바꿔요.
 * 입력: 변환할 HttpError인 error예요.
 * 출력: app/net 모듈 번호와 오류 번호를 담은 ErrorCode예요.
 * 실패: 없어요.
 */
inline kgc::app::errors::ErrorCode make(HttpError error)
{
    return {kModuleId, static_cast<std::uint16_t>(error)};
}
/** HTTP 오류를 화면 문구로 바꿔요.
 * 입력: 표시할 공통 오류 코드 error예요.
 * 출력: 현재 언어의 문구이며, 다른 모듈이나 모르는 번호는 빈 문자열이에요.
 * 실패: 없어요.
 */
QString errorText(kgc::app::errors::ErrorCode error);
}
