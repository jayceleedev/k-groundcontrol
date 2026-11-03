// [0.4.0 5.4 6.3] specs/features/0.4.0-sitl.md
#include "SitlCommand.h"
#include "SitlSources.h"
#include "Constants.h"
#include <kgc/comm/links/LinkConfig.h>
namespace kgc::sitl {
namespace {
// 범위 밖이거나 숫자가 아니면 참이에요. NaN은 비교가 모두 거짓이라 여기서 걸려요.
bool outside(double value, double min, double max) { return !(value >= min && value <= max); }
}
QStringList sitlArguments(SitlVehicle vehicle, const SitlHome& home, int speedup, bool wipe)
{
    // QString::number는 늘 C 지역 형식이라 독일어 환경에서도 소수점이 .이에요.
    const auto location = QStringList{
        QString::number(home.latitude, 'f', kLatLonDecimals), QString::number(home.longitude, 'f', kLatLonDecimals),
        QString::number(home.altitude, 'f', kAltitudeDecimals), QString::number(home.heading)}.join(QLatin1Char(','));
    QStringList arguments{
        QStringLiteral("--model"), sitlModel(vehicle), QStringLiteral("--home"), location,
        QStringLiteral("--speedup"), QString::number(speedup), QStringLiteral("-I0"),
        QStringLiteral("--serial0"), QString::fromLatin1(kUdpClientHost) + QString::number(kgc::comm::links::kUdpPort),
        QStringLiteral("--defaults"), QString::fromLatin1(kDefaultsFile)};
    if (wipe) arguments.append(QStringLiteral("--wipe"));
    return arguments;
}
QList<SitlHomeField> checkHome(const SitlHome& home)
{
    QList<SitlHomeField> fields;
    if (outside(home.latitude, settings::homeLatitudeMin(), settings::homeLatitudeMax())) fields.append(SitlHomeField::Latitude);
    if (outside(home.longitude, settings::homeLongitudeMin(), settings::homeLongitudeMax())) fields.append(SitlHomeField::Longitude);
    if (outside(home.altitude, settings::homeAltitudeMin(), settings::homeAltitudeMax())) fields.append(SitlHomeField::Altitude);
    if (outside(home.heading, settings::homeHeadingMin(), settings::homeHeadingMax())) fields.append(SitlHomeField::Heading);
    return fields;
}
SitlVehicle sitlVehicleFromSetting(settings::Vehicle value)
{
    switch (value) {
    case settings::Vehicle::Copter: return SitlVehicle::Copter;
    case settings::Vehicle::Heli: return SitlVehicle::Heli;
    case settings::Vehicle::Plane: return SitlVehicle::Plane;
    case settings::Vehicle::Rover: return SitlVehicle::Rover;
    }
    return SitlVehicle::Copter;
}
settings::Vehicle sitlVehicleToSetting(SitlVehicle vehicle)
{
    switch (vehicle) {
    case SitlVehicle::Copter: return settings::Vehicle::Copter;
    case SitlVehicle::Heli: return settings::Vehicle::Heli;
    case SitlVehicle::Plane: return settings::Vehicle::Plane;
    case SitlVehicle::Rover: return settings::Vehicle::Rover;
    }
    return settings::Vehicle::Copter;
}
SitlRelease sitlReleaseFromSetting(settings::Release value)
{
    switch (value) {
    case settings::Release::Stable: return SitlRelease::Stable;
    case settings::Release::Beta: return SitlRelease::Beta;
    case settings::Release::Latest: return SitlRelease::Latest;
    }
    return SitlRelease::Stable;
}
settings::Release sitlReleaseToSetting(SitlRelease release)
{
    switch (release) {
    case SitlRelease::Stable: return settings::Release::Stable;
    case SitlRelease::Beta: return settings::Release::Beta;
    case SitlRelease::Latest: return settings::Release::Latest;
    }
    return settings::Release::Stable;
}
}
