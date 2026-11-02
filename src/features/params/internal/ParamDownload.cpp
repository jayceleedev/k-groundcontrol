// [0.3.0 6.2 6.8-1 T5] specs/features/0.3.0-params.md
#include "ParamDownload.h"
#include "Constants.h"
#include <kgc/app/scheduler/Timeout.h>
#include <kgc/params/Errors.h>
#include <kgc/vehicle/outbound/VehicleSender.h>
#include <kgc/vehicle/registry/Vehicle.h>
#include <QCoreApplication>
namespace kgc::params {
using kgc::vehicle::outbound::OutboundKind;
QList<int> nextRequests(int total, const QSet<int>& received, QHash<int, int>& tries)
{
    QList<int> result;
    for (int index = 0; index < total && result.size() < kParamReadBatch; ++index) {
        if (received.contains(index) || tries.value(index) >= kParamReadTries) continue;
        ++tries[index];
        result.append(index);
    }
    return result;
}
ParamDownload::ParamDownload(kgc::vehicle::registry::Vehicle& vehicle, QObject* parent)
    : QObject(parent), vehicle_(vehicle), timer_(new kgc::app::scheduler::Timeout(QStringLiteral("paramDownload"), this))
{
    connect(timer_, &kgc::app::scheduler::Timeout::expired, this, [this] { started_ ? gapExpired() : listExpired(); });
}
void ParamDownload::start(kgc::app::job::Job* job)
{
    job_ = job;
    received_.clear();
    names_.clear();
    tries_.clear();
    total_ = 0;
    listRetries_ = 0;
    started_ = false;
    job->setStep(QString::fromUtf8(QT_TRANSLATE_NOOP("Job", "내려받기")));
    job->setProgress(0.0);
    job->setCancellable(true);
    connect(job, &kgc::app::job::Job::cancelRequested, this, [this] {
        finish();
        emit cancelled();
    });
    requestList();
}
bool ParamDownload::active() const { return job_ != nullptr; }
int ParamDownload::received() const { return received_.size(); }
int ParamDownload::total() const { return total_; }
void ParamDownload::requestList()
{
    timer_->start(kParamListTimeout);
    auto* sender = vehicle_.sender();
    if (!sender) return;
    // 송신 실패는 응답 없음과 같으므로 타이머가 다시 시도하게 둬요.
    sender->send(OutboundKind::Param, [target = vehicle_.sysid()](std::uint8_t sysid, std::uint8_t compid,
                                                                  std::uint8_t channel, mavlink_message_t& message) {
        mavlink_msg_param_request_list_pack_chan(sysid, compid, channel, &message, target, MAV_COMP_ID_AUTOPILOT1);
    });
}
void ParamDownload::listExpired()
{
    if (!vehicle_.sender() || listRetries_ >= kParamListRetries) {
        finish();
        emit failed(make(ParamError::Timeout));
        return;
    }
    ++listRetries_;
    requestList();
}
void ParamDownload::gapExpired()
{
    auto* sender = vehicle_.sender();
    if (!sender) {
        finish();
        emit failed(make(ParamError::Timeout));
        return;
    }
    const auto requests = nextRequests(total_, received_, tries_);
    if (requests.isEmpty()) {
        finish();
        emit failed(make(ParamError::Incomplete));
        return;
    }
    for (const int index : requests) {
        sender->send(OutboundKind::Param, [target = vehicle_.sysid(), index](std::uint8_t sysid, std::uint8_t compid,
                                                                             std::uint8_t channel, mavlink_message_t& message) {
            const char empty[kParamIdLength] = {};
            mavlink_msg_param_request_read_pack_chan(sysid, compid, channel, &message, target, MAV_COMP_ID_AUTOPILOT1,
                                                     empty, static_cast<std::int16_t>(index));
        });
    }
    timer_->start(kParamGapWait);
}
void ParamDownload::receive(const QString& name, int index, int count)
{
    if (!job_) return;
    if (index == kParamIndexNone) {
        if (started_) timer_->start(kParamGapWait);
        return;
    }
    started_ = true;
    total_ = count;
    if (index < count) {
        received_.insert(index);
        names_.insert(name);
    }
    if (total_ > 0) job_->setProgress(static_cast<double>(received_.size()) / total_);
    if (total_ > 0 && received_.size() >= total_) {
        finish();
        emit completed(names_);
        return;
    }
    timer_->start(kParamGapWait);
}
void ParamDownload::finish()
{
    timer_->stop();
    if (job_) disconnect(job_, nullptr, this, nullptr);
    job_ = nullptr;
}
}
