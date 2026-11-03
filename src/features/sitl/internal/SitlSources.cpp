// [0.4.0 5.4 6.2] specs/features/0.4.0-sitl.md
#include "SitlSources.h"
#include "Constants.h"
namespace kgc::sitl {
namespace {
QString releaseName(SitlRelease release)
{
    switch (release) {
    case SitlRelease::Stable: return QStringLiteral("stable");
    case SitlRelease::Beta: return QStringLiteral("beta");
    case SitlRelease::Latest: return QStringLiteral("latest");
    }
    return {};
}
// 주소의 +는 서버가 공백으로 읽지 않도록 %2B로 적어요.
QUrl encodedUrl(const QString& text)
{
    return QUrl(QString(text).replace(QLatin1Char('+'), QStringLiteral("%2B")));
}
QString windowsFolder(SitlVehicle vehicle, SitlRelease release)
{
    if (release == SitlRelease::Beta) return QString::fromLatin1(kWindowsBeta);
    if (release == SitlRelease::Latest) return {};
    switch (vehicle) {
    case SitlVehicle::Copter:
    case SitlVehicle::Heli: return QString::fromLatin1(kWindowsCopterStable);
    case SitlVehicle::Plane: return QString::fromLatin1(kWindowsPlaneStable);
    case SitlVehicle::Rover: return QString::fromLatin1(kWindowsRoverStable);
    }
    return {};
}
QString windowsExecutableStem(SitlVehicle vehicle)
{
    switch (vehicle) {
    case SitlVehicle::Copter: return QStringLiteral("ArduCopter");
    case SitlVehicle::Heli: return QStringLiteral("ArduHeli");
    case SitlVehicle::Plane: return QStringLiteral("ArduPlane");
    case SitlVehicle::Rover: return QStringLiteral("ArduRover");
    }
    return {};
}
// Linux 실행 파일의 상위 폴더(Copter·Plane·Rover)와 그 아래 경로예요.
QString linuxPath(SitlVehicle vehicle)
{
    const auto folder = QString::fromLatin1(kLinuxFolder);
    switch (vehicle) {
    case SitlVehicle::Copter: return QStringLiteral("Copter/%1/") + folder + QStringLiteral("/arducopter");
    case SitlVehicle::Heli: return QStringLiteral("Copter/%1/") + QString::fromLatin1(kLinuxHeliFolder) + QStringLiteral("/arducopter-heli");
    case SitlVehicle::Plane: return QStringLiteral("Plane/%1/") + folder + QStringLiteral("/arduplane");
    case SitlVehicle::Rover: return QStringLiteral("Rover/%1/") + folder + QStringLiteral("/ardurover");
    }
    return {};
}
QString paramsPath(SitlVehicle vehicle)
{
    switch (vehicle) {
    case SitlVehicle::Copter: return QString::fromLatin1(kCopterParams);
    case SitlVehicle::Heli: return QString::fromLatin1(kHeliParams);
    case SitlVehicle::Plane: return QString::fromLatin1(kPlaneParams);
    case SitlVehicle::Rover: return QString::fromLatin1(kRoverParams);
    }
    return {};
}
}
QList<SitlFile> sitlFiles(SitlPlatform platform, SitlVehicle vehicle, SitlRelease release)
{
    QList<SitlFile> files;
    if (platform == SitlPlatform::Windows) {
        const auto base = QString::fromLatin1(kWindowsBase) + windowsFolder(vehicle, release);
        const auto stem = windowsExecutableStem(vehicle);
        files.append({encodedUrl(base + stem + QString::fromLatin1(kWindowsExecutableSource)),
                      stem + QString::fromLatin1(kWindowsExecutable)});
        for (const char* dll : kWindowsDlls) {
            const auto name = QString::fromLatin1(dll);
            files.append({encodedUrl(base + name), name});
        }
    } else {
        const auto path = linuxPath(vehicle).arg(releaseName(release));
        files.append({encodedUrl(QString::fromLatin1(kLinuxBase) + path), path.section(QLatin1Char('/'), -1)});
    }
    files.append({encodedUrl(QString::fromLatin1(kParamsBase) + paramsPath(vehicle)), QString::fromLatin1(kDefaultsFile)});
    return files;
}
QString sitlFolderName(SitlPlatform platform, SitlVehicle vehicle, SitlRelease release)
{
    const auto system = platform == SitlPlatform::Windows ? QStringLiteral("windows") : QStringLiteral("linux");
    return system + QLatin1Char('-') + sitlRunFolderName(vehicle) + QLatin1Char('-') + releaseName(release);
}
QString sitlModel(SitlVehicle vehicle)
{
    return vehicle == SitlVehicle::Copter ? QStringLiteral("+") : sitlRunFolderName(vehicle);
}
QString sitlRunFolderName(SitlVehicle vehicle)
{
    switch (vehicle) {
    case SitlVehicle::Copter: return QStringLiteral("copter");
    case SitlVehicle::Heli: return QStringLiteral("heli");
    case SitlVehicle::Plane: return QStringLiteral("plane");
    case SitlVehicle::Rover: return QStringLiteral("rover");
    }
    return {};
}
}
