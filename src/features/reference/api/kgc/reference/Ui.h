// [0.0.4 T41 T42 T43] specs/features/0.0.4-righttabs.md
#pragma once
namespace kgc::ui { struct UiRegistry; }
namespace kgc::reference {
class ReferenceService;
/** 참고 패널을 메뉴에 등록해요.
 * 입력: 화면 등록부와 전역 서비스.
 * 출력: 없음.
 * 실패: 중복 등록은 프로그램 오류예요.
 */
void registerUi(kgc::ui::UiRegistry& registry, ReferenceService& service);
}
