// [0.2.0 T5] specs/features/0.2.0-fcflash.md
#include <kgc/vehicle/safety/ArmedPolicy.h>
#include <kgc/vehicle/registry/VehicleRegistry.h>
namespace kgc::vehicle::safety {
Decision decide(kgc::app::job::OperationKind kind, bool armed)
{
    // WORKER-FILL: spec §5.6 — BASE §9.2 표를 switch로 옮겨요. 시동이 아니면 Allow예요.
    Q_UNUSED(kind);
    Q_UNUSED(armed);
    return Decision::Allow;
}
ArmedPolicy::ArmedPolicy(kgc::vehicle::registry::VehicleRegistry& vehicles) : vehicles_(vehicles) {}
Decision ArmedPolicy::check(kgc::app::job::OperationKind kind, const VehicleScope& scope) const
{
    // WORKER-FILL: spec §5.6 — 범위의 기체에서 Arming.armed를 읽어 decide를 불러요.
    Q_UNUSED(kind);
    Q_UNUSED(scope);
    return Decision::Allow;
}
}
