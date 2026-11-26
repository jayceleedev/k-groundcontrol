// [0.5.0 S2 T8] specs/features/0.5.0-map.md
// [0.2.0 S3 S4] specs/features/0.2.0-fcflash.md
// [0.1.0 T8] specs/features/0.1.0-link.md
// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/app/boot/BootOptions.h>
#include <QCommandLineParser>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QUrl>
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
    parser.addOption({"tile-host", QT_TRANSLATE_NOOP("BootOptions", "타일 서버 주소(시험용)"), "url"});
    parser.addOption({"tile-ca",
        QT_TRANSLATE_NOOP("BootOptions", "타일 서버 인증서 PEM 파일(시험용, 이번 실행의 모든 HTTPS 요청이 믿음)"), "file"});
    parser.process(args);
    BootOptions options;
    options.extraSerial = parser.values("extra-serial");
    if (parser.isSet("tile-host")) {
        const auto value = parser.value("tile-host");
        const auto host = parseTileHost(value);
        if (!host)
            QCommandLineParser::showMessageAndExit(QCommandLineParser::MessageType::Error,
                QString(QT_TRANSLATE_NOOP("BootOptions", "타일 주소는 https://호스트[:포트] 꼴이어야 함: %1")).arg(value), 1);
        options.tileHost = *host;
    }
    options.tileCa = parser.value("tile-ca");
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
std::optional<QString> BootOptions::parseTileHost(const QString& value)
{
    const QUrl url(value, QUrl::StrictMode);
    const bool plainPath = url.path().isEmpty() || url.path() == u"/";
    if (!url.isValid() || url.scheme() != u"https" || url.host().isEmpty() || !plainPath || url.hasQuery()
        || url.hasFragment() || !url.userInfo().isEmpty())
        return std::nullopt;
    return url.adjusted(QUrl::RemovePath).toString();
}
void BootOptions::exitBadCertificate(const QString& path)
{
    // 번역기가 없는 부팅 단계라 다른 부팅 오류처럼 한국어 원문을 보여 줘요.
    QCommandLineParser::showMessageAndExit(QCommandLineParser::MessageType::Error,
        QString(QT_TRANSLATE_NOOP("BootOptions", "시험용 인증서를 읽을 수 없음: %1")).arg(path), 1);
}
}
