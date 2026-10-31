// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
#pragma once
namespace kgc::ui { struct UiRegistry; }
namespace kgc::link {
class LinkService;
/** 연결 부품과 상태 칩을 등록해요.
 * 입력: 화면 등록부와 전역 연결 서비스예요.
 * 출력: 없어요.
 * 실패: 중복 등록은 프로그래밍 오류예요.
 */
void registerUi(kgc::ui::UiRegistry& registry, LinkService& service);
}
