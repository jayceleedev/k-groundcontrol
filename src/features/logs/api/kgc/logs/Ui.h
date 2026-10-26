// [0.0.4 T4] specs/features/0.0.4-righttabs.md
#pragma once
namespace kgc::ui { struct UiRegistry; }
namespace kgc::logs {
class LogsViewer;
/** 우 사이드바의 로그 탭을 등록해요.
 * 입력: 화면 등록부와 전역 뷰어.
 * 출력: 없음.
 * 실패: 고정 id가 겹치는 중복 등록은 프로그래밍 오류예요.
 */
void registerUi(kgc::ui::UiRegistry& registry, LogsViewer& viewer);
}
