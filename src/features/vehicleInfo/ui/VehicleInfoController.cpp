// [0.1.0 T9 T10 T11 T12 T13 T16 T17 T20] specs/features/0.1.0-link.md
#include "VehicleInfoController.h"
#include <cmath>
namespace kgc::vehicleInfo {
VehicleInfoController::VehicleInfoController(kgc::vehicle::registry::VehicleRegistry& vehicles)
    : QObject(&vehicles), vehicles_(vehicles)
{
    Q_ASSERT(!instance_);
    instance_ = this;
    connect(&vehicles_, &kgc::vehicle::registry::VehicleRegistry::activeChanged, this, &VehicleInfoController::rebind);
    rebind();
}
VehicleInfoController* VehicleInfoController::create(QQmlEngine* engine, QJSEngine*)
{
    Q_ASSERT(instance_ && instance_->thread() == engine->thread());
    QQmlEngine::setObjectOwnership(instance_, QQmlEngine::CppOwnership);
    return instance_;
}
void VehicleInfoController::rebind()
{
    if (vehicle_) disconnect(vehicle_->states(), nullptr, this, nullptr);
    vehicle_ = vehicles_.active();
    if (vehicle_) {
        using kgc::vehicle::states::VehicleStates;
        for (auto signal : {&VehicleStates::linkChanged, &VehicleStates::attitudeChanged,
             &VehicleStates::positionChanged, &VehicleStates::armingChanged, &VehicleStates::gpsChanged,
             &VehicleStates::batteryChanged})
            connect(vehicle_->states(), signal, this, &VehicleInfoController::valuesChanged);
    }
    emit valuesChanged();
}
kgc::vehicle::states::AttitudeBundle VehicleInfoController::attitude() const
{
    return vehicle_ ? vehicle_->states()->attitude() : kgc::vehicle::states::AttitudeBundle{};
}
kgc::vehicle::states::PositionBundle VehicleInfoController::position() const
{
    return vehicle_ ? vehicle_->states()->position() : kgc::vehicle::states::PositionBundle{};
}
kgc::vehicle::states::ArmingBundle VehicleInfoController::arming() const
{
    return vehicle_ ? vehicle_->states()->arming() : kgc::vehicle::states::ArmingBundle{};
}
kgc::vehicle::states::BatteryBundle VehicleInfoController::battery() const
{
    return vehicle_ ? vehicle_->states()->battery() : kgc::vehicle::states::BatteryBundle{};
}
kgc::vehicle::states::GpsBundle VehicleInfoController::gps() const
{
    return vehicle_ ? vehicle_->states()->gps() : kgc::vehicle::states::GpsBundle{};
}
bool VehicleInfoController::hasVehicle() const { return bool(vehicle_); }
bool VehicleInfoController::available() const
{
    const auto value = attitude();
    return vehicle_ && !vehicle_->states()->link().lost
        && std::isfinite(value.rollDeg) && std::isfinite(value.pitchDeg);
}
double VehicleInfoController::roll() const { return attitude().rollDeg; }
double VehicleInfoController::pitch() const { return attitude().pitchDeg; }
double VehicleInfoController::heading() const { return attitude().yawDeg; }
double VehicleInfoController::speed() const { return position().groundSpeedMs; }
double VehicleInfoController::altitude() const { return position().altRelM; }
double VehicleInfoController::altAmsl() const { return position().altAmslM; }
double VehicleInfoController::climbRate() const { return position().climbMs; }
QString VehicleInfoController::modeName() const { return arming().modeName; }
qint64 VehicleInfoController::modeId() const { return arming().modeId; }
bool VehicleInfoController::armed() const { return arming().armed; }
double VehicleInfoController::batteryVoltage() const { return battery().voltageV; }
double VehicleInfoController::batteryCurrent() const { return battery().currentA; }
double VehicleInfoController::batteryPercent() const { return battery().remainingPct < 0 ? qQNaN() : battery().remainingPct; }
int VehicleInfoController::fixType() const { return gps().fixType; }
double VehicleInfoController::satellites() const { return gps().satellites < 0 ? qQNaN() : gps().satellites; }
bool VehicleInfoController::udp() const { return !vehicle_ || vehicle_->linkConfig().kind == kgc::comm::links::LinkConfig::Kind::Udp; }
QString VehicleInfoController::portName() const { return vehicle_ ? vehicle_->linkConfig().portName : QString{}; }
int VehicleInfoController::baudRate() const { return vehicle_ ? vehicle_->linkConfig().baudRate : 0; }
}
