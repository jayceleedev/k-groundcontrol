// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#pragma once
namespace kgc::ui { struct UiRegistry; }
namespace kgc::appSettings {
class AppSettingsService;
/** 화면의 컨트롤러와 기여를 등록해요.
 * 입력: 화면 등록부와 서비스.
 * 출력: 없음.
 * 실패: 없음.
 */
void registerUi(kgc::ui::UiRegistry& registry, AppSettingsService& service);
}
