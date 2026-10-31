// [0.1.0 T8] specs/features/0.1.0-link.md
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
    // 번역 목록에는 넣되, 번역기가 없는 부팅 단계에서는 한국어 원문을 보여 줘요.
    parser.addOption({"log-level", QT_TRANSLATE_NOOP("BootOptions", "로그 수준"), "level"});
    parser.addOption({"data-dir", QT_TRANSLATE_NOOP("BootOptions", "데이터 폴더"), "path"});
    parser.addOption({"extra-serial", QT_TRANSLATE_NOOP("BootOptions", "추가 시리얼 경로"), "path"});
    parser.process(args);
    BootOptions options;
    options.extraSerial = parser.value("extra-serial");
    const auto levelName = parser.isSet("log-level") ? parser.value("log-level")
        : ini.value("log/level", "info").toString();
    const auto level = kgc::app::log::parseLogLevel(levelName);
    if (!level && parser.isSet("log-level"))
        QCommandLineParser::showMessageAndExit(QCommandLineParser::MessageType::Error,
            QString(QT_TRANSLATE_NOOP("BootOptions", "지원하지 않는 로그 수준: %1")).arg(levelName), 1);
    // 파일 오타 때문에 앱이 열리지 않는 일을 막도록 기본 수준으로 돌아가요.
    options.logLevel = level.value_or(kgc::app::log::LogLevel::Info);
    options.maxFileMB = qMax(1, ini.value("log/maxFileMB", 5).toInt());
    options.maxFiles = qMax(1, ini.value("log/maxFiles", 5).toInt());
    options.dataDir = parser.isSet("data-dir") ? parser.value("data-dir")
        : ini.value("paths/dataDir").toString();
    if (options.dataDir.isEmpty())
        options.dataDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    return options;
}
}
