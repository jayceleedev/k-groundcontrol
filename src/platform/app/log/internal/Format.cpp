// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/app/log/LogSink.h>
#include <QDateTime>
#include <array>
namespace kgc::app::log {
namespace {
struct LevelEntry { LogLevel level; QtMsgType qt; const char* label; };
// 설정 이름과 Qt 수준과 출력 표기가 어긋나지 않도록 한 표에서만 대응시켜요.
constexpr std::array levels{
    LevelEntry{LogLevel::Debug, QtDebugMsg, "DEBUG"},
    LevelEntry{LogLevel::Info, QtInfoMsg, "INFO"},
    LevelEntry{LogLevel::Warn, QtWarningMsg, "WARN"},
    LevelEntry{LogLevel::Error, QtCriticalMsg, "ERROR"}
};
}
std::optional<LogLevel> parseLogLevel(QStringView name)
{
    for (const auto& entry : levels)
        if (name == QString::fromLatin1(entry.label).toLower()) return entry.level;
    return std::nullopt;
}
LogLevel LogSink::fromQtLevel(QtMsgType level)
{
    for (const auto& entry : levels)
        if (entry.qt == level) return entry.level;
    // Qt의 치명적 오류도 앱 로그에서는 가장 높은 오류 수준으로 기록해요.
    return LogLevel::Error;
}
QString LogSink::format(LogLevel level, const QString& category,
                        const QString& event, const QString& message)
{
    const auto label = QString::fromLatin1(levels.at(static_cast<std::size_t>(level)).label);
    auto escaped = message;
    escaped.replace('\r', "\\r").replace('\n', "\\n");
    return QDateTime::currentDateTime().toOffsetFromUtc(
        QDateTime::currentDateTime().offsetFromUtc()).toString(Qt::ISODateWithMs)
        + ' ' + label.leftJustified(5) + ' ' + category.leftJustified(16)
        + ' ' + event.leftJustified(24) + ' ' + escaped + '\n';
}
}
