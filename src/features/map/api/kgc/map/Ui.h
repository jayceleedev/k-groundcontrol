// [0.5.0 5.6.2 5.6.9] specs/features/0.5.0-map.md
#pragma once
namespace kgc::ui { struct UiRegistry; }
namespace kgc::map {
class MapService;
/** 지도 부품이 쓸 타일 공급을 QML 엔진에 넘기고 컨트롤러를 만들어요.
 * 입력: 화면 등록부와 전역 지도 서비스예요. 셸 창을 만들기 전에 한 번 불러요.
 * 출력: 엔진 프로퍼티 kgcTileSource에 타일 공급을 두고(공급이 사라지면 비워요), MapController를 서비스 아래에 만들어요.
 * 실패: 두 번 부르는 것은 프로그래밍 오류예요.
 */
void registerUi(kgc::ui::UiRegistry& registry, MapService& service);
}
