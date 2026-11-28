// [0.6.0 5.11.2 5.11.11] specs/features/0.6.0-fly.md
#pragma once
namespace kgc::vehicle::registry { class VehicleRegistry; }
namespace kgc::ui { struct UiRegistry; }
namespace kgc::fly {
/** 상단바 비행 단추 여덟을 등록하고 컨트롤러를 만들어요(앱 시작 때 한 번).
 * 입력: 화면 등록부와 활성 기체를 알려 주는 기체 등록부예요.
 * 출력: 없어요. 컨트롤러는 기체 등록부가 소유하고 활성 기체에 다시 묶여요.
 * 실패: 같은 id의 중복 등록은 프로그래밍 오류예요.
 */
void registerUi(kgc::ui::UiRegistry& registry, kgc::vehicle::registry::VehicleRegistry& vehicles);
}
