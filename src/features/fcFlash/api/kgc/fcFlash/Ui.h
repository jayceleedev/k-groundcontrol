// [0.2.0 T14] specs/features/0.2.0-fcflash.md
#pragma once
namespace kgc::ui { struct UiRegistry; }
namespace kgc::fcFlash {
class FcFlashService;
/** 펌웨어 세팅 패널을 메뉴에 등록하고 컨트롤러를 만들어요.
 * 입력: 화면 등록부와 전역 서비스예요.
 * 출력: 없어요.
 * 실패: 중복 등록은 프로그래밍 오류예요.
 */
void registerUi(kgc::ui::UiRegistry& registry, FcFlashService& service);
}
