// [0.3.0 5.5] specs/features/0.3.0-params.md
#pragma once
namespace kgc::vehicle::registry { class VehicleRegistry; }
namespace kgc::ui { struct UiRegistry; }
namespace kgc::params {
/** 수동 기체 설정 패널을 메뉴에 등록하고 컨트롤러를 만들어요.
 * 입력: 화면 등록부와 활성 기체를 제공하는 기체 등록부예요.
 * 출력: 없어요.
 * 실패: 중복 등록은 프로그래밍 오류예요.
 */
void registerUi(kgc::ui::UiRegistry& registry, kgc::vehicle::registry::VehicleRegistry& vehicles);
}
