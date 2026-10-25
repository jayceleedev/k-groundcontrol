// [0.0.2 T26] specs/features/0.0.2-theme.md
#pragma once

namespace kgc::ui {
/** 창을 만들기 전에 공용 멀티샘플 형식을 준비해요.
 * 입력: 없음. 창 생성 전에 호출해요.
 * 출력: 기본 표면 형식의 멀티샘플 수를 4로 설정해요.
 * 실패: 없음.
 */
void applyDefaultSurfaceFormat();
}
