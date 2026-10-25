// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/app/log/LogSink.h>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
namespace kgc::app::log {
namespace {
struct Sink {
    QFile file;
    QMutex mutex;
    QString directory;
    LogLevel threshold = LogLevel::Info;
    qint64 maxBytes = 0;
    int maxFiles = 0;
    QtMessageHandler previous = nullptr;
    bool installed = false;
    ~Sink() { if (installed) qInstallMessageHandler(previous); }
};
Sink& sink() { static Sink value; return value; }
bool rotate(Sink& state)
{
    state.file.close();
    QDir dir(state.directory);
    QFile::remove(dir.filePath(QString("kgc.%1.log").arg(state.maxFiles)));
    for (int index = state.maxFiles - 1; index > 0; --index)
        QFile::rename(dir.filePath(QString("kgc.%1.log").arg(index)),
                      dir.filePath(QString("kgc.%1.log").arg(index + 1)));
    QFile::rename(state.file.fileName(), dir.filePath("kgc.1.log"));
    return state.file.open(QIODevice::WriteOnly | QIODevice::Append);
}
}
bool LogSink::install(const QString& dir, LogLevel level, int maxFileMB, int maxFiles)
{
    auto& state = sink();
    QMutexLocker lock(&state.mutex);
    if (!QDir().mkpath(dir)) return false;
    state.file.close();
    state.directory = dir;
    state.file.setFileName(QDir(dir).filePath("kgc.log"));
    state.maxBytes = qint64(maxFileMB) * (1 << 20);
    state.maxFiles = maxFiles;
    state.threshold = level;
    if (!state.file.open(QIODevice::WriteOnly | QIODevice::Append)) return false;
    if (!state.installed) {
        state.previous = qInstallMessageHandler([](QtMsgType type, const QMessageLogContext&, const QString& text) {
            LogSink::write(fromQtLevel(type), "kgc.qt", "qt.Message", text);
        });
        state.installed = true;
    }
    return true;
}
void LogSink::write(LogLevel level, const QString& category, const QString& event, const QString& message)
{
    auto& state = sink();
    QMutexLocker lock(&state.mutex);
    if (level < state.threshold || !state.file.isOpen()) return;
    if (state.file.size() > state.maxBytes && !rotate(state)) return;
    state.file.write(format(level, category, event, message).toUtf8());
    state.file.flush();
}
}
