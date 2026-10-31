// [0.2.0 T2] specs/features/0.2.0-fcflash.md
#include <kgc/app/job/Job.h>
#include <kgc/app/job/Errors.h>
#include "Constants.h"
#include <QElapsedTimer>
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>
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
    QElapsedTimer clock;
    std::vector<Releaser> releasers;
    bool ended() const { return state != State::Pending && state != State::Running; }
    void releaseAll()
    {
        while (!releasers.empty()) {
            auto release = std::move(releasers.back());
            releasers.pop_back();
            if (release) release();
        }
    }
    void finish(Job& job, State terminal, kgc::app::errors::Result<void> result)
    {
        state = terminal;
        releaseAll();
        if (cancellable) {
            cancellable = false;
            emit job.cancellableChanged();
        }
        emit job.stateChanged();
        emit job.finished(result);
    }
};
Job::Job(OperationKind kind, QObject* parent) : QObject(parent), d_(std::make_unique<Private>())
{
    d_->kind = kind;
}
Job::~Job() { d_->releaseAll(); }
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
    if (d_->ended()) return;
    if (d_->state == State::Pending) {
        d_->clock.start();
        d_->state = State::Running;
        emit stateChanged();
    }
    if (d_->stepName == key) return;
    d_->stepName = key;
    emit stepNameChanged();
}
void Job::setProgress(double value)
{
    if (d_->ended()) return;
    d_->progress = std::isfinite(value) && value >= 0 ? std::clamp(value, 0.0, 1.0) : kUnknownProgress;
    d_->etaSeconds = kUnknownEta;
    if (d_->clock.isValid() && d_->progress >= kMinProgressForEta) {
        const double elapsed = d_->clock.elapsed() / 1000.0;
        const double remaining = elapsed * (1.0 - d_->progress) / d_->progress;
        d_->etaSeconds = static_cast<int>(std::min(std::ceil(remaining), double(std::numeric_limits<int>::max())));
    }
    emit progressChanged();
}
void Job::setCancellable(bool value)
{
    if (d_->ended() || d_->cancellable == value) return;
    d_->cancellable = value;
    emit cancellableChanged();
}
void Job::succeed()
{
    if (d_->ended()) return;
    d_->finish(*this, State::Succeeded, {});
}
void Job::fail(kgc::app::errors::ErrorCode code, const QString& text)
{
    if (d_->ended()) return;
    d_->errorCode = code;
    d_->errorText = text;
    d_->finish(*this, State::Failed, std::unexpected(code));
}
void Job::cancel() { if (!d_->ended()) emit cancelRequested(); }
void Job::finishCancelled()
{
    if (d_->ended()) return;
    const auto code = make(JobError::Cancelled);
    d_->errorCode = code;
    d_->errorText = kgc::app::job::errorText(code);
    d_->finish(*this, State::Cancelled, std::unexpected(code));
}
void Job::hold(Releaser releaser)
{
    if (d_->ended()) { if (releaser) releaser(); }
    else d_->releasers.push_back(std::move(releaser));
}
}
