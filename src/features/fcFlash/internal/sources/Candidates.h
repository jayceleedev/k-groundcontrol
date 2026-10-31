// [0.2.0 T12] specs/features/0.2.0-fcflash.md
#pragma once
namespace kgc::fcFlash {
/** 후보 수에 따른 선택 방식이에요.
 * 입력: 없어요.
 * 출력: None(없음, 파일 선택 안내), Auto(자동 선택), Choose(사용자가 고름) 중 하나예요.
 * 실패: 없어요.
 */
enum class CandidatePick { None, Auto, Choose };
/** 후보 수로 선택 방식을 정해요.
 * 입력: 후보 수 count예요.
 * 출력: 0이면 None, 1이면 Auto, 그보다 많으면 Choose예요.
 * 실패: 없어요.
 */
CandidatePick pickCandidate(int count);
}
