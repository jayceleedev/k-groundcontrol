// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/app/boot/BootOptions.h>
#include <QCommandLineParser>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
namespace kgc::app::boot {
BootOptions BootOptions::parse(const QStringList& args)
{
    QSettings ini(QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
                      .filePath("kgc.ini"), QSettings::IniFormat);
    QCommandLineParser parser;
    // 터미널용 설명은 화면 번역과 분리해 한국어 원문으로 유지해요.
    parser.addOption({"log-level", "로그 수준", "level"});
    parser.addOption({"data-dir", "데이터 폴더", "path"});
    parser.process(args);
    BootOptions options;
    const auto levelName = parser.isSet("log-level") ? parser.value("log-level")
        : ini.value("log/level", "info").toString();
    const auto level = kgc::app::log::parseLogLevel(levelName);
    if (!level)
        QCommandLineParser::showMessageAndExit(QCommandLineParser::MessageType::Error,
            QString("지원하지 않는 로그 수준: %1").arg(levelName), 1);
    options.logLevel = *level;
    options.maxFileMB = qMax(1, ini.value("log/maxFileMB", 5).toInt());
    options.maxFiles = qMax(1, ini.value("log/maxFiles", 5).toInt());
    options.dataDir = parser.isSet("data-dir") ? parser.value("data-dir")
        : ini.value("paths/dataDir").toString();
    if (options.dataDir.isEmpty())
        options.dataDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    return options;
}
}
