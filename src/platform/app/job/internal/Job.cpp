// [0.2.0 T2] specs/features/0.2.0-fcflash.md
#include <kgc/app/job/Job.h>
#include <kgc/app/job/Errors.h>
#include "Constants.h"
namespace kgc::app::job {
struct Job::Private {
    OperationKind kind;
    State state = State::Pending;
    QString stepName;
    double progress = kUnknownProgress;
    int etaSeconds = kUnknownEta;
    bool cancellable = false;
    std::optional<kgc::app::errors::ErrorCode> errorCode;
    QString errorText;
};
Job::Job(OperationKind kind, QObject* parent) : QObject(parent), d_(std::make_unique<Private>())
{
    d_->kind = kind;
}
Job::~Job()
{
    // WORKER-FILL: spec §5.3 — 남은 해제 핸들을 역순으로 불러요.
}
OperationKind Job::kind() const { return d_->kind; }
Job::State Job::state() const { return d_->state; }
QString Job::stepName() const { return d_->stepName; }
double Job::progress() const { return d_->progress; }
int Job::etaSeconds() const { return d_->etaSeconds; }
bool Job::cancellable() const { return d_->cancellable; }
std::optional<kgc::app::errors::ErrorCode> Job::errorCode() const { return d_->errorCode; }
QString Job::errorText() const { return d_->errorText; }
void Job::setStep(const QString& key)
{
    // WORKER-FILL: spec §5.3 — stepName을 바꾸고, 첫 호출이면 Running으로 바꾸며 경과 시계를 시작해요.
    Q_UNUSED(key);
}
void Job::setProgress(double value)
{
    // WORKER-FILL: spec §5.3 — progress를 바꾸고 etaSeconds를 경과와 진행률로 셈해요. 1% 미만이면 -1이에요.
    Q_UNUSED(value);
}
void Job::setCancellable(bool value)
{
    // WORKER-FILL: spec §5.3 — cancellable을 바꾸고 cancellableChanged를 내요.
    Q_UNUSED(value);
}
void Job::succeed()
{
    // WORKER-FILL: spec §5.3 — Succeeded로 끝내고, 해제 핸들을 역순으로 부른 뒤 finished를 한 번 내요.
}
void Job::fail(kgc::app::errors::ErrorCode code, const QString& text)
{
    // WORKER-FILL: spec §5.3 — Failed로 끝내고 code·text를 남기며, 해제 뒤 finished를 한 번 내요.
    Q_UNUSED(code);
    Q_UNUSED(text);
}
void Job::cancel()
{
    // WORKER-FILL: spec §5.3 — cancelRequested만 내요. 상태는 바꾸지 않아요.
}
void Job::finishCancelled()
{
    // WORKER-FILL: spec §5.3 — Cancelled로 끝내고, 해제 뒤 finished(make(JobError::Cancelled))를 한 번 내요.
}
void Job::hold(Releaser releaser)
{
    // WORKER-FILL: spec §5.3 — 해제 핸들을 쥐어 두고, 끝날 때 맡긴 순서의 역순으로 불러요.
    Q_UNUSED(releaser);
}
}
