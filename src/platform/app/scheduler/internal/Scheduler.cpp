// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/app/scheduler/Scheduler.h>
#include <QThread>
#include <QTimer>
#include <atomic>
#include <memory>
namespace kgc::app::scheduler {
Scheduler::Scheduler(QObject* parent) : QObject(parent) {}
void Scheduler::registerTask(QString name, std::chrono::milliseconds period, QObject* owner,
                             QObject* context, std::function<void()> fn)
{
    if (thread() != QThread::currentThread() || !owner || !context || !fn || period.count() <= 0
        || !owners_.value(name).isNull())
        qFatal("Invalid scheduler registration");
    Q_ASSERT(owner->thread() == context->thread());
    owners_.insert(name, owner);
    auto live = std::make_shared<std::atomic_bool>(true);
    QObject::connect(owner, &QObject::destroyed, this, [live] { live->store(false); }, Qt::DirectConnection);
    QObject::connect(this, &QObject::destroyed, owner, [live] { live->store(false); }, Qt::DirectConnection);
    // 생성 전 소유자가 사라진 경우에도 예약된 실행이 남지 않도록 표식을 공유해요.
    QMetaObject::invokeMethod(context, [live, period, context, fn = std::move(fn)] {
        if (!live->load()) return;
        auto* timer = new QTimer(context);
        QObject::connect(timer, &QTimer::timeout, context, [live, timer, fn] {
            if (live->load()) fn();
            else timer->deleteLater();
        });
        timer->start(period);
    }, Qt::QueuedConnection);
}
}
