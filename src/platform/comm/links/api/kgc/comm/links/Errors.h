// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/comm/links/ModuleId.h>
#include <QCoreApplication>
namespace kgc::comm::links {
/** 열기와 통신 실패를 구분하는 오류 번호예요.
 * 입력: 중복 연결, 포트 열기·권한·점유·UDP 바인딩·연결 유실 상황이에요.
 * 출력: 각 상황에 대응하는 LinkError 값이에요.
 * 실패: 없어요.
 */
enum class LinkError : std::uint16_t {
    AlreadyConnected = 1, PortOpenFailed, PermissionDenied, PortBusy, UdpBindFailed, PortLost
};
/** 링크 오류를 공통 코드로 바꿔요.
 * 입력: 변환할 LinkError 값 error예요.
 * 출력: comm/links 모듈 번호와 해당 오류 번호를 담은 ErrorCode예요.
 * 실패: 없어요.
 */
inline kgc::app::errors::ErrorCode make(LinkError error)
{
    return {kModuleId, static_cast<std::uint16_t>(error)};
}
/** 링크 오류를 번역 가능한 문구로 바꿔요.
 * 입력: 표시할 공통 오류 코드 error예요.
 * 출력: 현재 언어의 오류 문구이며, 다른 모듈이나 모르는 번호는 빈 문자열이에요.
 * 실패: 없어요.
 */
QString errorText(kgc::app::errors::ErrorCode error);
}
