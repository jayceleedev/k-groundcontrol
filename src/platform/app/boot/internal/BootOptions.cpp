// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/app/boot/BootOptions.h>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
namespace kgc::app::boot {
BootOptions BootOptions::parse(const QStringList& args)
{
    QSettings ini(QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
                      .filePath("kgc.ini"), QSettings::IniFormat);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption({"log-level", QCoreApplication::translate("BootOptions", "로그 수준"), "level"});
    parser.addOption({"data-dir", QCoreApplication::translate("BootOptions", "데이터 폴더"), "path"});
    parser.process(args);
    BootOptions options;
    options.logLevel = parser.isSet("log-level") ? parser.value("log-level")
        : ini.value("log/level", "info").toString();
    options.maxFileMB = qMax(1, ini.value("log/maxFileMB", 5).toInt());
    options.maxFiles = qMax(1, ini.value("log/maxFiles", 5).toInt());
    options.dataDir = parser.isSet("data-dir") ? parser.value("data-dir")
        : ini.value("paths/dataDir").toString();
    if (options.dataDir.isEmpty())
        options.dataDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    return options;
}
}
