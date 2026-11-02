// [0.3.0 6.1–6.7 6.8-1] specs/features/0.3.0-params.md
#include <kgc/params/ParamService.h>
#include "Constants.h"
#include "Events.h"
#include "ParamCodec.h"
#include "ParamDownload.h"
#include "ParamFile.h"
#include "ParamGrouping.h"
#include "ParamMetadataLoader.h"
#include <kgc/params/ParamRebootReconnect.h>
#include "ParamSearch.h"
#include "ParamValue.h"
#include "ParamWrite.h"
#include <kgc/app/log/Log.h>
#include <kgc/app/paths/Paths.h>
#include <kgc/app/scheduler/Timeout.h>
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/params/Errors.h>
#include <kgc/vehicle/command/CommandQueue.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <QDateTime>
#include <QFile>
#include <QSaveFile>
#include <algorithm>
namespace kgc::params {
using kgc::app::job::Job;
using kgc::app::job::OperationKind;
using kgc::vehicle::firmware::ParamMetadataFormat;
using kgc::vehicle::safety::Decision;
using kgc::vehicle::safety::VehicleScope;
namespace {
bool writeWhole(const QString& path, const QByteArray& bytes)
{
    QSaveFile file(path);
    return file.open(QIODevice::WriteOnly) && file.write(bytes) == bytes.size() && file.commit();
}
}
ParamService::ParamService(kgc::vehicle::registry::Vehicle& vehicle, kgc::vehicle::safety::ArmedPolicy& policy,
                           kgc::app::net::Http& http, ParamRebootReconnect& reconnect, QObject* parent)
    : QObject(parent), vehicle_(vehicle), policy_(policy), params_(new Params(this)),
      download_(new ParamDownload(vehicle, this)), write_(new ParamWrite(vehicle, this)),
      metadata_(new ParamMetadataLoader(http, this)), reconnect_(reconnect),
      regroupTimer_(new kgc::app::scheduler::Timeout(QStringLiteral("paramRegroup"), this))
{
    auto* states = vehicle.states();
    linkLost_ = states->link().lost;
    connect(&vehicle, &kgc::vehicle::registry::Vehicle::messageReceived, this, &ParamService::receive);
    connect(states, &kgc::vehicle::states::VehicleStates::linkChanged, this, &ParamService::linkUpdated);
    connect(states, &kgc::vehicle::states::VehicleStates::capabilitiesChanged, this, &ParamService::startMetadata);
    connect(download_, &ParamDownload::completed, this, [this](const QSet<QString>& names) {
        params_->removeExcept(names);
        finishDownload(DownloadState::Complete);
        KGC_LOG_INFO(kCategory, ParamEvent::DownloadFinished, "Params downloaded: %1", params_->rowCount());
        job_->succeed();
    });
    connect(download_, &ParamDownload::failed, this, [this](kgc::app::errors::ErrorCode error) {
        finishDownload(DownloadState::Incomplete);
        KGC_LOG_WARN(kCategory, ParamEvent::DownloadFailed, "Params download failed: %1 of %2",
                     download_->received(), download_->total());
        job_->fail(error, errorText(error));
    });
    connect(download_, &ParamDownload::cancelled, this, [this] {
        finishDownload(DownloadState::Idle);
        job_->finishCancelled();
    });
    connect(write_, &ParamWrite::written, this, [this](const QString& name) {
        const auto* meta = params_->meta(name);
        if (meta && meta->rebootRequired) setRebootNeeded(true);
    });
    connect(write_, &ParamWrite::ended, this, [this](int count, const QStringList& failed) {
        KGC_LOG_INFO(kCategory, ParamEvent::WriteFinished, "Params written: %1, failed: %2", count, failed.size());
        emit writeFinished(count, failed);
    });
    connect(metadata_, &ParamMetadataLoader::loaded, this, [this](const QHash<QString, ParamMeta>& metadata) {
        params_->setMetadata(metadata);
        regroup();
        KGC_LOG_INFO(kCategory, ParamEvent::MetadataLoaded, "Param metadata loaded: %1", metadata.size());
        setMetadataState(MetadataState::Ready);
    });
    connect(metadata_, &ParamMetadataLoader::failed, this, [this] {
        KGC_LOG_WARN(kCategory, ParamEvent::MetadataFailed, "Param metadata failed");
        setMetadataState(MetadataState::Failed);
    });
    connect(regroupTimer_, &kgc::app::scheduler::Timeout::expired, this, [this] {
        regroupPending_ = false;
        regroup();
    });
    startMetadata();
    autoRefresh();
}
ParamService::~ParamService()
{
    refreshDeferred_ = false;
    if (job_) job_->fail(make(ParamError::Timeout), errorText(make(ParamError::Timeout)));
}
ParamService* ParamService::of(kgc::vehicle::registry::Vehicle* vehicle)
{
    return vehicle ? vehicle->findChild<ParamService*>(QString(), Qt::FindDirectChildrenOnly) : nullptr;
}
Params* ParamService::params() const { return params_; }
DownloadState ParamService::downloadState() const { return downloadState_; }
MetadataState ParamService::metadataState() const { return metadataState_; }
Job* ParamService::job() const { return job_; }
bool ParamService::rebootNeeded() const { return rebootNeeded_; }
int ParamService::receivedCount() const { return download_->received(); }
int ParamService::totalCount() const { return download_->total(); }
void ParamService::refresh()
{
    if (job_) return;
    auto* job = beginJob(OperationKind::Read);
    setDownloadState(DownloadState::Downloading);
    KGC_LOG_INFO(kCategory, ParamEvent::DownloadStarted, "Params download started");
    download_->start(job);
}
void ParamService::autoRefresh()
{
    if (job_) {
        refreshDeferred_ = true;
        return;
    }
    if (vehicle_.states()->arming().armed) setDownloadState(DownloadState::SkippedArmed);
    else refresh();
}
Job* ParamService::beginJob(OperationKind kind)
{
    auto* job = new Job(kind, this);
    job_ = job;
    connect(job, &Job::finished, this, [this, job] {
        job_ = nullptr;
        job->deleteLater();
        emit jobChanged();
        if (refreshDeferred_) {
            refreshDeferred_ = false;
            autoRefresh();
        }
    });
    emit jobChanged();
    return job;
}
void ParamService::finishDownload(DownloadState state)
{
    regroup();
    setDownloadState(state);
}
void ParamService::receive(const mavlink_message_t& message)
{
    if (message.msgid != MAVLINK_MSG_ID_PARAM_VALUE) return;
    mavlink_param_value_t decoded{};
    mavlink_msg_param_value_decode(&message, &decoded);
    const auto name = QString::fromLatin1(decoded.param_id, qstrnlen(decoded.param_id, kParamIdLength));
    if (name == QStringLiteral("_HASH_CHECK")) return;
    const auto& plugin = vehicle_.plugin();
    const double value = decodeParam(decoded.param_value, decoded.param_type, plugin.paramEncoding());
    const auto* meta = params_->meta(name);
    const bool added = params_->store(name, value, decoded.param_type, plugin.paramGroup(name, meta ? meta->group : QString{}));
    // 한 번에 여러 행이 늘어도 전체 묶음 계산은 한 번만 해요.
    if (added && !download_->active() && !regroupPending_) {
        regroupPending_ = true;
        regroupTimer_->start(kParamRegroupDelay);
    }
    download_->receive(name, decoded.param_index, decoded.param_count);
    write_->receive(name, value);
}
void ParamService::regroup()
{
    const auto& plugin = vehicle_.plugin();
    QStringList groups;
    groups.reserve(params_->rowCount());
    for (const auto& row : params_->all()) {
        const auto* meta = params_->meta(row.name);
        groups.append(plugin.paramGroup(row.name, meta ? meta->group : QString{}));
    }
    params_->setGroups(mergeSingleGroups(groups));
}
void ParamService::startMetadata()
{
    const auto capabilities = vehicle_.states()->capabilities();
    if (metadataStarted_ || capabilities.vehicleType < 0 || capabilities.firmwareVersion.isEmpty()) return;
    metadataStarted_ = true;
    const auto source = vehicle_.plugin().paramMetadataSource(capabilities.vehicleType, capabilities.firmwareVersion);
    if (source.format == ParamMetadataFormat::None) return;
    setMetadataState(MetadataState::Loading);
    metadata_->start(source);
}
void ParamService::linkUpdated()
{
    const bool lost = vehicle_.states()->link().lost;
    const bool returned = linkLost_ && !lost;
    linkLost_ = lost;
    if (!returned || (downloadState_ != DownloadState::Incomplete && !rebootSent_)) return;
    rebootSent_ = false;
    autoRefresh();
}
kgc::app::errors::Result<void> ParamService::setPending(const QString& name, double value)
{
    const auto* row = params_->find(name);
    if (!row) return std::unexpected(make(ParamError::NotFound));
    const auto* meta = params_->meta(name);
    if (meta && meta->readOnly) return std::unexpected(make(ParamError::ReadOnly));
    params_->setPending(name, sameValue(value, row->value, row->type) ? std::nullopt : std::optional(value));
    return {};
}
void ParamService::clearPending(const QString& name) { params_->setPending(name, std::nullopt); }
Decision ParamService::writeDecision() const
{
    const auto kind = params_->pendingCount() > 1 ? OperationKind::ParamBulk : OperationKind::ParamSingle;
    return policy_.check(kind, VehicleScope::One(&vehicle_));
}
QByteArray ParamService::fileBytes() const
{
    const auto capabilities = vehicle_.states()->capabilities();
    return writeParamFile({vehicle_.sysid(), capabilities.firmwareName, capabilities.vehicleType, capabilities.firmwareVersion},
                          params_->all());
}
void ParamService::writePending()
{
    if (job_) return;
    QList<ParamWriteItem> items;
    for (const auto& row : params_->all())
        if (row.pending) items.append({row.name, *row.pending, row.type});
    if (items.isEmpty()) {
        emit writeFinished(0, {});
        return;
    }
    const bool bulk = items.size() > 1;
    auto* job = beginJob(bulk ? OperationKind::ParamBulk : OperationKind::ParamSingle);
    const auto folder = kgc::app::paths::Paths::paramsBackups();
    const auto path = folder + QStringLiteral("/%1-%2.params").arg(vehicle_.sysid())
        .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss")));
    if (folder.isEmpty() || !writeWhole(path, fileBytes())) {
        job->fail(make(ParamError::BackupFailed), errorText(make(ParamError::BackupFailed)));
        return;
    }
    KGC_LOG_INFO(kCategory, ParamEvent::BackupWritten, "Param backup written: %1", path);
    write_->start(job, std::move(items), bulk);
}
kgc::app::errors::Result<void> ParamService::saveFile(const QString& path) const
{
    if (!writeWhole(path, fileBytes())) return std::unexpected(make(ParamError::FileWrite));
    return {};
}
kgc::app::errors::Result<FileDiff> ParamService::compareFile(const QString& path) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return std::unexpected(make(ParamError::FileOpen));
    const auto parsed = parseParamFile(file.readAll());
    if (!parsed) return std::unexpected(parsed.error());
    FileDiff diff;
    diff.badLines = parsed->badLines;
    diff.otherComponent = parsed->otherComponent;
    const auto skip = vehicle_.plugin().paramFileSkip();
    for (const auto& [name, value] : parsed->entries) {
        const auto* row = params_->find(name);
        const auto* meta = params_->meta(name);
        if (skip.contains(name)) ++diff.skipped;
        else if (!row) ++diff.missing;
        else if (meta && meta->readOnly) ++diff.readOnly;
        else if (!validateForType(value, row->type)) ++diff.badLines;
        else if (sameValue(value, row->value, row->type)) ++diff.same;
        else diff.changed.append({name, row->value, value});
    }
    std::sort(diff.changed.begin(), diff.changed.end(),
              [](const FileDiffRow& a, const FileDiffRow& b) { return naturalLess(a.name, b.name); });
    return diff;
}
void ParamService::applyFile(const QStringList& names, const FileDiff& diff)
{
    for (const auto& row : diff.changed)
        if (names.contains(row.name)) (void)setPending(row.name, row.fileValue);
}
void ParamService::reboot()
{
    if (job_) {
        emit rebootFinished(std::unexpected(make(ParamError::Busy)));
        return;
    }
    if (policy_.check(OperationKind::ParamBulk, VehicleScope::One(&vehicle_)) == Decision::Deny) {
        emit rebootFinished(std::unexpected(make(ParamError::Armed)));
        return;
    }
    kgc::vehicle::command::CommandLong command;
    command.command = MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN;
    command.param1 = 1.0F;
    vehicle_.commands()->send(command, {.retries = 0},
                              [this](const kgc::vehicle::command::CommandResult& result) { rebootAnswered(result); }, this);
}
void ParamService::rebootAnswered(const kgc::vehicle::command::CommandResult& result)
{
    using kgc::vehicle::command::CommandFailure;
    if (result.failure == CommandFailure::Gone) return;
    // 재부팅 때 ACK 없이 바로 끊기는 FC가 있어서 응답 없음도 성공으로 봐요.
    const bool sent = result.result ? *result.result == MAV_RESULT_ACCEPTED : result.failure == CommandFailure::Timeout;
    if (!sent) {
        emit rebootFinished(std::unexpected(make(ParamError::RebootRefused)));
        return;
    }
    KGC_LOG_INFO(kCategory, ParamEvent::RebootSent, "Reboot sent");
    setRebootNeeded(false);
    rebootSent_ = true;
    reconnect_.watch(vehicle_.linkConfig());
    emit rebootFinished({});
}
void ParamService::setDownloadState(DownloadState state)
{
    if (downloadState_ == state) return;
    downloadState_ = state;
    emit downloadStateChanged();
}
void ParamService::setMetadataState(MetadataState state)
{
    if (metadataState_ == state) return;
    metadataState_ = state;
    emit metadataStateChanged();
}
void ParamService::setRebootNeeded(bool value)
{
    if (rebootNeeded_ == value) return;
    rebootNeeded_ = value;
    emit rebootNeededChanged();
}
}
