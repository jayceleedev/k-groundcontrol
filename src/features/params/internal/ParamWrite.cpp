// [0.3.0 6.6 6.8-1] specs/features/0.3.0-params.md
#include "ParamWrite.h"
#include "Constants.h"
#include "Events.h"
#include "ParamCodec.h"
#include "ParamValue.h"
#include <kgc/app/log/Log.h>
#include <kgc/app/scheduler/Timeout.h>
#include <kgc/params/Errors.h>
#include <kgc/vehicle/outbound/VehicleSender.h>
#include <kgc/vehicle/registry/Vehicle.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <QCoreApplication>
#include <algorithm>
#include <array>
#include <cstring>
namespace kgc::params {
using kgc::app::job::Job;
ParamWrite::ParamWrite(kgc::vehicle::registry::Vehicle& vehicle, QObject* parent)
    : QObject(parent), vehicle_(vehicle), timer_(new kgc::app::scheduler::Timeout(QStringLiteral("paramWrite"), this))
{
    connect(timer_, &kgc::app::scheduler::Timeout::expired, this, &ParamWrite::expired);
}
void ParamWrite::start(Job* job, QList<ParamWriteItem> items, bool bulk)
{
    job_ = job;
    items_ = std::move(items);
    failed_.clear();
    current_ = 0;
    count_ = 0;
    bulk_ = bulk;
    cancelRequested_ = false;
    job->setStep(QString::fromUtf8(QT_TRANSLATE_NOOP("Job", "값 쓰기")));
    job->setProgress(0.0);
    job->setCancellable(true);
    connect(job, &Job::cancelRequested, this, [this] { cancelRequested_ = true; });
    next();
}
void ParamWrite::next()
{
    auto* job = job_.data();
    if (cancelRequested_) {
        release();
        emit ended(count_, failed_);
        job->finishCancelled();
        return;
    }
    if (current_ >= items_.size()) {
        release();
        emit ended(count_, failed_);
        job->succeed();
        return;
    }
    if (bulk_ && vehicle_.states()->arming().armed) {
        release();
        emit ended(count_, failed_);
        job->fail(make(ParamError::Armed), errorText(make(ParamError::Armed)));
        return;
    }
    attempts_ = 0;
    transmit();
}
void ParamWrite::transmit()
{
    timer_->start(kParamSetTimeout);
    auto* sender = vehicle_.sender();
    if (!sender) return;
    const auto& item = items_[current_];
    std::array<char, kParamIdLength + 1> id{};
    const auto name = item.name.toLatin1();
    std::memcpy(id.data(), name.constData(), std::min<std::size_t>(name.size(), kParamIdLength));
    const float wire = encodeParam(item.value, item.type, vehicle_.plugin().paramEncoding());
    // 송신 실패는 응답 없음과 같으므로 타이머가 다시 시도하게 둬요.
    sender->send(kgc::vehicle::outbound::OutboundKind::Param,
        [target = vehicle_.sysid(), id, wire, type = item.type](std::uint8_t sysid, std::uint8_t compid,
                                                                std::uint8_t channel, mavlink_message_t& message) {
            mavlink_msg_param_set_pack_chan(sysid, compid, channel, &message, target, MAV_COMP_ID_AUTOPILOT1,
                                            id.data(), wire, type);
        });
}
void ParamWrite::receive(const QString& name, double value)
{
    if (!job_ || current_ >= items_.size() || items_[current_].name != name) return;
    timer_->stop();
    const auto& item = items_[current_];
    if (!sameValue(value, item.value, item.type)) {
        retry();
        return;
    }
    ++count_;
    emit written(name);
    advance();
}
void ParamWrite::expired()
{
    if (!vehicle_.sender()) {
        auto* job = job_.data();
        release();
        if (job) job->fail(make(ParamError::Timeout), errorText(make(ParamError::Timeout)));
        return;
    }
    retry();
}
void ParamWrite::retry()
{
    if (attempts_ < kParamSetRetries) {
        ++attempts_;
        transmit();
        return;
    }
    KGC_LOG_WARN(kCategory, ParamEvent::SetFailed, "Param set failed: %1", items_[current_].name);
    failed_.append(items_[current_].name);
    advance();
}
void ParamWrite::advance()
{
    ++current_;
    job_->setProgress(static_cast<double>(current_) / items_.size());
    next();
}
void ParamWrite::release()
{
    timer_->stop();
    if (job_) disconnect(job_, nullptr, this, nullptr);
    job_ = nullptr;
}
}
