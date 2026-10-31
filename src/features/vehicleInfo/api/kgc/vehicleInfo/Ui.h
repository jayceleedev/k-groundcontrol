// [0.1.0 T9 T10 T11 T12 T13 T16 T17 T20] specs/features/0.1.0-link.md
// [0.0.4 T3 T4] specs/features/0.0.4-righttabs.md
#pragma once
namespace kgc::vehicle::registry { class VehicleRegistry; }
namespace kgc::ui { struct UiRegistry; }
namespace kgc::vehicleInfo {
/** 우 사이드바의 계기 칸과 개요·상세·메시지 탭을 등록해요.
 * 입력: 화면 등록부와 활성 기체를 제공하는 기체 등록부예요.
 * 출력: 없음.
 * 실패: 고정 id·자리가 겹치는 중복 등록은 프로그래밍 오류예요.
 */
void registerUi(kgc::ui::UiRegistry& registry, kgc::vehicle::registry::VehicleRegistry& vehicles);
}
