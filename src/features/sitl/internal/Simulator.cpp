// [0.4.0 5.4 6.5 6.6] specs/features/0.4.0-sitl.md
#include "Simulator.h"
#include "Constants.h"
#include <QProcess>
#include <utility>
namespace kgc::sitl {
Simulator::Simulator(QObject* parent) : QObject(parent) {}
Simulator::~Simulator()
{
    stop();
}
void Simulator::start(const QString& program, const QStringList& arguments, const QString& workingDirectory)
{
    Q_ASSERT(!running());
    delete process_;
    pending_.clear();
    requested_ = false;
    process_ = new QProcess(this);
    process_->setProcessChannelMode(QProcess::MergedChannels);
    process_->setWorkingDirectory(workingDirectory);
    auto* process = process_;
    connect(process, &QProcess::started, this, [this, process] { emit started(process->processId()); });
    connect(process, &QProcess::readyReadStandardOutput, this, &Simulator::readLines);
    connect(process, &QProcess::errorOccurred, this, [this, process](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) emit failedToStart(process->errorString());
    });
    connect(process, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus status) {
        readLines();
        if (!pending_.isEmpty()) emit line(QString::fromLocal8Bit(std::exchange(pending_, {})));
        emit exited(exitCode, status == QProcess::CrashExit, requested_);
    });
    process_->start(program, arguments);
}
void Simulator::stop()
{
    if (!running()) return;
    requested_ = true;
    process_->kill();
    process_->waitForFinished(int(kSitlStopWait.count()));
}
bool Simulator::running() const
{
    return process_ && process_->state() != QProcess::NotRunning;
}
void Simulator::readLines()
{
    if (!process_) return;
    pending_.append(process_->readAllStandardOutput());
    for (auto end = pending_.indexOf('\n'); end >= 0; end = pending_.indexOf('\n')) {
        emit line(QString::fromLocal8Bit(pending_.left(end)));
        pending_.remove(0, end + 1);
    }
}
}
