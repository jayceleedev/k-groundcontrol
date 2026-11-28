// [0.6.0 5.5] specs/features/0.6.0-fly.md
// [0.4.2 5.1] specs/features/0.4.2-param-tree.md
// [0.3.0 5.2] specs/features/0.3.0-params.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
namespace kgc::vehicle::firmware {
class GenericPlugin final : public FirmwarePlugin {
public:
    QString displayName() const override;
    QString modeName(std::uint32_t customMode, std::uint8_t mavType) const override;
    QList<StreamRequest> streamRequests() const override;
    ParamEncoding paramEncoding() const override;
    ParamMetadataSource paramMetadataSource(int mavType, const QString& version) const override;
    QString paramGroupTitle(const QString& prefix) const override;
    QStringList paramFileSkip() const override;
    QList<MessageInterval> messageIntervals() const override;
    bool vtolStateDecidesClass(int mavType) const override;
    QList<std::uint32_t> launchOnArmModes(VehicleClass vehicleClass) const override;
    QList<FlightMode> flightModes(VehicleClass vehicleClass) const override;
    std::optional<std::uint32_t> guidedMode(VehicleClass vehicleClass) const override;
    QList<CommandStep> setModeSteps(VehicleClass vehicleClass, std::uint32_t customMode) const override;
    QList<CommandStep> armSteps() const override;
    QList<CommandStep> disarmSteps() const override;
    QList<CommandStep> emergencyStopSteps() const override;
    TakeoffKind takeoffKind(VehicleClass vehicleClass) const override;
    QString takeoffAltitudeParam() const override;
    kgc::app::geo::AltitudeFrame takeoffAltitudeFrame(VehicleClass vehicleClass) const override;
    QList<CommandStep> takeoffSteps(VehicleClass vehicleClass, double altitude, bool armed) const override;
    QList<CommandStep> landSteps(VehicleClass vehicleClass) const override;
    QList<CommandStep> rtlSteps(VehicleClass vehicleClass) const override;
    QList<CommandStep> pauseSteps(VehicleClass vehicleClass, FlightShape shape, const QString& firmwareVersion) const override;
    kgc::app::geo::AltitudeFrame goToAltitudeFrame() const override;
    QList<CommandStep> goToSteps(VehicleClass vehicleClass, double latDeg, double lonDeg, double altitude) const override;
    QString loiterRadiusParam() const override;
    QString vtolHoverParam() const override;
    bool loitersAtTarget(VehicleClass vehicleClass, FlightShape shape, std::optional<double> hoverParam) const override;
    LoiterCircle loiterCircle(VehicleClass vehicleClass, FlightShape shape, double orbitRadiusM,
                              std::optional<double> radiusParam, std::optional<double> hoverParam) const override;
};
}
