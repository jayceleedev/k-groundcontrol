// [0.0.3 T1 T2 T3 T9 T12 T13 T14] specs/features/0.0.3-windows.md
#pragma once
namespace kgc::ui { struct UiRegistry; }
namespace kgc::analyze {
class AppLogService;
/** 하단 단추와 창 컨트롤러를 등록해요.
 * 입력: 화면 등록부와 전역 서비스.
 * 출력: 없음.
 * 실패: 중복 등록은 프로그래밍 오류예요.
 */
void registerUi(kgc::ui::UiRegistry& registry, AppLogService& service);
}
