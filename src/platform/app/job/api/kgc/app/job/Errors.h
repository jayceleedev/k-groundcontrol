// [0.2.0 T2] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/job/ModuleId.h>
#include <QString>
namespace kgc::app::job {
/** 작업이 오류로 끝나지 않고 멈춘 까닭을 나타내요.
 * 입력: 취소를 끝낸 작업의 상황이에요.
 * 출력: JobError::Cancelled 값이에요.
 * 실패: 없어요.
 */
enum class JobError : std::uint16_t { Cancelled = 1 };
/** 작업 오류를 공통 코드로 바꿔요.
 * 입력: 변환할 JobError인 error예요.
 * 출력: app/job 모듈 번호와 오류 번호를 담은 ErrorCode예요.
 * 실패: 없어요.
 */
inline kgc::app::errors::ErrorCode make(JobError error)
{
    return {kModuleId, static_cast<std::uint16_t>(error)};
}
/** 작업 오류를 화면 문구로 바꿔요.
 * 입력: 표시할 공통 오류 코드 error예요.
 * 출력: 현재 언어의 문구이며, 다른 모듈이나 모르는 번호는 빈 문자열이에요.
 * 실패: 없어요.
 */
QString errorText(kgc::app::errors::ErrorCode error);
}
