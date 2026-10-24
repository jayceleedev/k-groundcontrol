// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/app/log/LogSink.h>
#include <QDateTime>
namespace kgc::app::log {
QString LogSink::format(QtMsgType level, const QString& category,
                        const QString& event, const QString& message)
{
    const QString label = level == QtDebugMsg ? "DEBUG" : level == QtInfoMsg ? "INFO"
        : level == QtWarningMsg ? "WARN" : "ERROR";
    auto escaped = message;
    escaped.replace('\r', "\\r").replace('\n', "\\n");
    return QDateTime::currentDateTime().toOffsetFromUtc(
        QDateTime::currentDateTime().offsetFromUtc()).toString(Qt::ISODateWithMs)
        + ' ' + label.leftJustified(5) + ' ' + category.leftJustified(16)
        + ' ' + event.leftJustified(24) + ' ' + escaped + '\n';
}
}
