// [0.5.0 S4 T11] specs/features/0.5.0-map.md
// [0.2.0 T5] specs/features/0.2.0-fcflash.md
#include <kgc/vehicle/safety/ArmedPolicy.h>
#include <kgc/vehicle/registry/VehicleRegistry.h>
namespace kgc::vehicle::safety {
Decision decide(kgc::app::job::OperationKind kind, bool armed)
{
    using kgc::app::job::OperationKind;
    if (!armed) return Decision::Allow;
    switch (kind) {
    case OperationKind::SetupPage:
    case OperationKind::FcFlash:
    case OperationKind::EscFlash:
    case OperationKind::ElrsFlash:
    case OperationKind::TrackerFlash:
    case OperationKind::EscConfig:
    case OperationKind::ElrsConfig:
    case OperationKind::AiWrite:
    case OperationKind::ParamBulk:
    case OperationKind::LayoutWrite:
    case OperationKind::MotorTest: return Decision::Deny;
    case OperationKind::ParamSingle:
    case OperationKind::Tuning:
    case OperationKind::Autotune: return Decision::AllowWithWarning;
    case OperationKind::TrackerConfig:
    case OperationKind::TrackerCalib:
    case OperationKind::TrackerManual:
    case OperationKind::FlyAction:
    case OperationKind::MissionStartStop:
    case OperationKind::Read:
    // 기체와 무관한 작업이라 check를 부르지 않지만, 표를 빠짐없이 두려고 허용으로 둬요(BASE §9.2).
    case OperationKind::MapDownload: return Decision::Allow;
    }
    return Decision::Deny;
}
ArmedPolicy::ArmedPolicy(kgc::vehicle::registry::VehicleRegistry& vehicles) : vehicles_(vehicles) {}
Decision ArmedPolicy::check(kgc::app::job::OperationKind kind, const VehicleScope& scope) const
{
    if (!scope.any()) {
        const auto* vehicle = scope.vehicle();
        return decide(kind, vehicle && vehicle->states()->arming().armed);
    }
    for (const auto& vehicle : vehicles_.vehicles())
        if (vehicle && vehicle->states()->arming().armed) return decide(kind, true);
    return decide(kind, false);
}
}
