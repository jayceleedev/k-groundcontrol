// [0.3.0 5.5 7] specs/features/0.3.0-params.md
#pragma once
#include <kgc/params/ModuleId.h>
#include <QString>
namespace kgc::params {
/** 파라미터 작업이 멈춘 까닭을 나타내요.
 * 입력: 스펙 §6·§7의 실패 상황이에요.
 * 출력: 각 상황에 대응하는 ParamError 값이에요.
 * 실패: 없어요.
 */
enum class ParamError : std::uint16_t {
    Timeout = 1, Incomplete, BackupFailed, Armed, NotFound, NotANumber, NotInteger, TypeLimit, ReadOnly,
    FileOpen, FileEmpty, FileWrite, Busy, RebootRefused
};
/** 파라미터 오류를 공통 코드로 바꿔요.
 * 입력: 변환할 ParamError인 error예요.
 * 출력: params 모듈 번호와 오류 번호를 담은 ErrorCode예요.
 * 실패: 없어요.
 */
inline kgc::app::errors::ErrorCode make(ParamError error)
{
    return {kModuleId, static_cast<std::uint16_t>(error)};
}
/** 파라미터 오류를 화면 문구로 바꿔요.
 * 입력: 표시할 공통 오류 코드 error예요.
 * 출력: 현재 언어의 문구이며, 다른 모듈의 코드는 빈 문자열이에요.
 * 실패: 없어요.
 */
QString errorText(kgc::app::errors::ErrorCode error);
}
