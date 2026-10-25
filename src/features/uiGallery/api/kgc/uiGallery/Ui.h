// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
#pragma once
namespace kgc::ui { struct UiRegistry; }
namespace kgc::uiGallery {
class UiGalleryService;
/** 부품 목록 패널을 메뉴에 등록해요.
 * 입력: 화면 등록부와 전역 서비스.
 * 출력: 없음.
 * 실패: 중복 등록은 프로그램 오류예요.
 */
void registerUi(kgc::ui::UiRegistry& registry, UiGalleryService& service);
}
