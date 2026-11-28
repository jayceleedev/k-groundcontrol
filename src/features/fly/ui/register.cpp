// [0.6.0 5.11.11] specs/features/0.6.0-fly.md
#include <kgc/fly/Ui.h>
#include <kgc/ui/UiRegistry.h>
#include "FlyController.h"
namespace kgc::fly {
void registerUi(kgc::ui::UiRegistry& registry, kgc::vehicle::registry::VehicleRegistry& vehicles)
{
    // 0.0.4 §15.1의 차례·묶음 그대로 상단바 가운데에 놓아요.
    const auto top = [&registry](const char* id, int order, int group, const char* file) {
        const auto result = registry.topbar.registerItem(QString::fromLatin1(id), order, QStringLiteral("centre"), group,
            QUrl(QStringLiteral("qrc:/qt/qml/Kgc/Fly/") + QLatin1StringView(file)));
        Q_ASSERT(result.has_value());
    };
    top("fly.vehicle", 1, 1, "FlyVehicle.qml");
    top("fly.mode", 2, 2, "FlyMode.qml");
    top("fly.arm", 3, 2, "FlyArm.qml");
    top("fly.takeoff", 4, 3, "FlyTakeoff.qml");
    top("fly.land", 5, 3, "FlyLand.qml");
    top("fly.rtl", 6, 3, "FlyRtl.qml");
    top("fly.pause", 7, 3, "FlyPause.qml");
    top("fly.emergencyStop", 8, 4, "FlyEmergencyStop.qml");
    // 목표·선회는 홈(20)과 기체(30) 사이라 화살표가 위에 오고, 여기로 가기 창은 다른 레이어 위예요.
    const auto layer = [&registry](const char* id, int order, const char* file) {
        const auto result = registry.mapLayers.registerLayer(QString::fromLatin1(id), order,
            QUrl(QStringLiteral("qrc:/qt/qml/Kgc/Fly/") + QLatin1StringView(file)));
        Q_ASSERT(result.has_value());
    };
    layer("flyTarget", 25, "FlyTargetLayer.qml");
    layer("flyGoTo", 90, "FlyGoToLayer.qml");
    new FlyController(vehicles, registry.topbar);
}
}
