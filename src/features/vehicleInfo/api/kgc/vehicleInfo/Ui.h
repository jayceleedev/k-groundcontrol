// [0.0.4 T3 T4] specs/features/0.0.4-righttabs.md
#pragma once
namespace kgc::ui { struct UiRegistry; }
namespace kgc::vehicleInfo {
/** 우 사이드바의 계기 칸과 개요·상세·메시지 탭을 등록해요.
 * 입력: 화면 등록부(전역 부분이 없는 기체별 모듈이라 서비스 인자가 없어요).
 * 출력: 없음.
 * 실패: 고정 id·자리가 겹치는 중복 등록은 프로그래밍 오류예요.
 */
void registerUi(kgc::ui::UiRegistry& registry);
}
