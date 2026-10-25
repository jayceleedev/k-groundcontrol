// [0.0.3 T5 T7 T8] specs/features/0.0.3-windows.md
#include <kgc/app/log/LogFeed.h>
#include "Constants.h"
#include <QMutexLocker>
#include <QScopedValueRollback>
namespace kgc::app::log {
thread_local bool LogFeed::delivering_ = false;
LogFeed::LogFeed(QObject* parent) : LogFeed(kFeedCapacity, parent) {}
LogFeed::LogFeed(int capacity, QObject* parent) : QObject(parent), capacity_(capacity)
{
    Q_ASSERT(capacity > 0);
}
void LogFeed::publish(LogRecord record)
{
    QMutexLocker lock(&mutex_);
    record.seq = ++last_;
    if (records_.size() == capacity_) records_.removeFirst();
    records_.append(record);
    QMetaObject::invokeMethod(this, [this, record] {
        // 직접 연결된 화면 처리 중 발생한 경고가 다시 화면을 갱신하지 않게 해요.
        QScopedValueRollback guard(delivering_, true);
        emit recordAdded(record);
    }, Qt::QueuedConnection);
}
std::pair<QList<LogRecord>, quint64> LogFeed::recent() const
{
    QMutexLocker lock(&mutex_);
    return {records_, last_};
}
int LogFeed::capacity() const { return capacity_; }
}
