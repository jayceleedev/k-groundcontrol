// [0.5.0 S1 T10] specs/features/0.5.0-map.md
#pragma once
#include <kgc/app/store/ModuleId.h>
#include <QString>
namespace kgc::app::store {
/** 덩이 저장소가 실패한 까닭을 나타내요.
 * 입력: 저장소 파일을 열거나 준비하지 못한 상황, 열린 뒤 읽기·쓰기 문장이 실패한 상황이에요.
 * 출력: OpenFailed와 QueryFailed 값이에요.
 * 실패: 없어요.
 */
enum class StoreError : std::uint16_t { OpenFailed = 1, QueryFailed };
/** 저장소 오류를 공통 코드로 바꿔요.
 * 입력: 변환할 StoreError인 error예요.
 * 출력: app/store 모듈 번호와 오류 번호를 담은 ErrorCode예요.
 * 실패: 없어요.
 */
inline kgc::app::errors::ErrorCode make(StoreError error)
{
    return {kModuleId, static_cast<std::uint16_t>(error)};
}
/** 저장소 오류를 화면 문구로 바꿔요.
 * 입력: 표시할 공통 오류 코드 error예요.
 * 출력: 번역 문맥 "Store"의 지금 언어 문구이며, 다른 모듈이나 모르는 번호는 빈 문자열이에요.
 * 실패: 없어요.
 */
QString errorText(kgc::app::errors::ErrorCode error);
}
