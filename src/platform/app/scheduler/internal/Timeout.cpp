// [0.2.0 T6] specs/features/0.2.0-fcflash.md
#include <kgc/app/scheduler/Timeout.h>
#include <QTimer>
namespace kgc::app::scheduler {
struct Timeout::Private {
    QTimer* timer;
};
Timeout::Timeout(QString name, QObject* parent) : QObject(parent), d_(std::make_unique<Private>())
{
    d_->timer = new QTimer(this);
    d_->timer->setObjectName(std::move(name));
    d_->timer->setSingleShot(true);
    d_->timer->setTimerType(Qt::PreciseTimer);
    connect(d_->timer, &QTimer::timeout, this, &Timeout::expired);
}
Timeout::~Timeout() = default;
void Timeout::start(std::chrono::milliseconds duration) { d_->timer->start(duration); }
void Timeout::stop() { d_->timer->stop(); }
}
