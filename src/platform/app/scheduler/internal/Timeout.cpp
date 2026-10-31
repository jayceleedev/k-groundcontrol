// [0.2.0 T6] specs/features/0.2.0-fcflash.md
#include <kgc/app/scheduler/Timeout.h>
namespace kgc::app::scheduler {
struct Timeout::Private {
    QString name;
};
Timeout::Timeout(QString name, QObject* parent) : QObject(parent), d_(std::make_unique<Private>())
{
    d_->name = std::move(name);
}
Timeout::~Timeout() = default;
void Timeout::start(std::chrono::milliseconds duration)
{
    // WORKER-FILL: spec §5.2 — 단발 QTimer를 (다시) 시작하고 끝나면 expired를 한 번 내요.
    Q_UNUSED(duration);
}
void Timeout::stop()
{
    // WORKER-FILL: spec §5.2 — 재는 중인 QTimer를 멈춰요.
}
}
