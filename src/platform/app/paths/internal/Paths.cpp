// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/app/paths/Paths.h>
#include <QDir>
#include <QStandardPaths>
namespace kgc::app::paths {
namespace { QString root; }
void Paths::init(const QString& dataDir)
{
    root = QDir(dataDir.isEmpty() ? QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation) : dataDir).absolutePath();
}
QString Paths::dataDir() { return root; }
QString Paths::directory(const QString& relative)
{
    const auto path = QDir(root).filePath(relative);
    return QDir().mkpath(path) ? path : QString{};
}
QString Paths::logsApp() { return directory(QStringLiteral("logs/app")); }
QString Paths::logsTlog() { return directory(QStringLiteral("logs/tlog")); }
QString Paths::logsOnboard() { return directory(QStringLiteral("logs/onboard")); }
QString Paths::missions() { return directory(QStringLiteral("missions")); }
QString Paths::layouts() { return directory(QStringLiteral("layouts")); }
QString Paths::paramsBackups() { return directory(QStringLiteral("params/backups")); }
QString Paths::paramsCache() { return directory(QStringLiteral("params/cache")); }
QString Paths::aiChanges() { return directory(QStringLiteral("ai/changes")); }
QString Paths::video() { return directory(QStringLiteral("video")); }
QString Paths::cacheFirmware() { return directory(QStringLiteral("cache/firmware")); }
QString Paths::cacheTiles() { return directory(QStringLiteral("cache/tiles")); }
QString Paths::cacheMetadata() { return directory(QStringLiteral("cache/metadata")); }
QString Paths::cacheTerrain() { return directory(QStringLiteral("cache/terrain")); }
}
