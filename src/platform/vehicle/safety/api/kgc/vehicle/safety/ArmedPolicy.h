// [0.2.0 T5] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/job/OperationKind.h>
#include <kgc/vehicle/safety/VehicleScope.h>
namespace kgc::vehicle::registry { class VehicleRegistry; }
namespace kgc::vehicle::safety {
/** 작업을 시작해도 되는지의 판정이에요.
 * 입력: 없어요.
 * 출력: Allow(시작), AllowWithWarning(길게 눌러 확인), Deny(거부) 중 하나예요.
 * 실패: 없어요.
 */
enum class Decision { Allow, AllowWithWarning, Deny };
/** BASE §9.2 표를 그대로 옮긴 순수 판정이에요.
 * 입력: 작업 종류 kind와 시동 여부 armed예요.
 * 출력: 시동이 아니면 Allow, 시동 중이면 표의 값이에요.
 * 실패: 없어요.
 */
Decision decide(kgc::app::job::OperationKind kind, bool armed);
/** 범위 안 기체의 시동 상태로 작업 시작을 판정해요.
 * 입력: 기체 목록을 읽을 등록부예요.
 * 출력: Decision이에요.
 * 실패: 없어요.
 */
class ArmedPolicy {
public:
/** 판정기를 만들어요.
 * 입력: 수명 동안 유지되는 vehicles예요.
 * 출력: 등록부를 읽기만 하는 판정기예요.
 * 실패: 없어요.
 */
    explicit ArmedPolicy(kgc::vehicle::registry::VehicleRegistry& vehicles);
/** 지금 시작해도 되는지 판정해요.
 * 입력: 작업 종류 kind와 범위 scope예요. Any는 연결된 기체 중 하나라도 시동 중이면 시동 중으로 봐요.
 * 출력: decide(kind, armed)의 값이에요.
 * 실패: One의 기체가 지워졌으면 시동이 아닌 것으로 봐요.
 */
    Decision check(kgc::app::job::OperationKind kind, const VehicleScope& scope) const;
private:
    kgc::vehicle::registry::VehicleRegistry& vehicles_;
};
}
