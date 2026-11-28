// [0.4.2 5.5] specs/features/0.4.2-param-tree.md
// [0.3.0 6.8 6.8-1] specs/features/0.3.0-params.md
#include "ParamsController.h"
#include "Constants.h"
#include "ParamSearch.h"
#include "ParamTree.h"
#include "ParamTreeModel.h"
#include "ParamValue.h"
#include "ParamsFilterModel.h"
#include <kgc/app/paths/Paths.h>
#include <kgc/params/Errors.h>
#include <kgc/vehicle/firmware/FirmwarePlugin.h>
#include <QCoreApplication>
#include <algorithm>
namespace kgc::params {
namespace {
QString text(const char* key) { return QCoreApplication::translate("Params", key); }
// 트리는 Params가 드는 순서(자연순) 그대로의 이름으로 만들어요.
QStringList paramNames(const Params* params)
{
    QStringList names;
    if (!params) return names;
    names.reserve(params->all().size());
    for (const auto& entry : params->all()) names.append(entry.name);
    return names;
}
}
ParamsController::ParamsController(kgc::vehicle::registry::VehicleRegistry& vehicles)
    : QObject(&vehicles), vehicles_(vehicles), filter_(new ParamsFilterModel(this)), tree_(new ParamTreeModel(this))
{
    Q_ASSERT(!instance_);
    instance_ = this;
    connect(&vehicles_, &kgc::vehicle::registry::VehicleRegistry::activeChanged, this, &ParamsController::rebind);
    connect(filter_, &QAbstractItemModel::rowsInserted, this, &ParamsController::visibleCountChanged);
    connect(filter_, &QAbstractItemModel::rowsRemoved, this, &ParamsController::visibleCountChanged);
    connect(filter_, &QAbstractItemModel::modelReset, this, &ParamsController::visibleCountChanged);
    connect(filter_, &QAbstractItemModel::layoutChanged, this, &ParamsController::visibleCountChanged);
    rebind();
}
ParamsController* ParamsController::create(QQmlEngine* engine, QJSEngine*)
{
    Q_ASSERT(instance_ && instance_->thread() == engine->thread());
    QQmlEngine::setObjectOwnership(instance_, QQmlEngine::CppOwnership);
    return instance_;
}
void ParamsController::rebind()
{
    if (service_) {
        disconnect(service_, nullptr, this, nullptr);
        disconnect(service_->params(), nullptr, this, nullptr);
    }
    vehicle_ = vehicles_.active();
    service_ = ParamService::of(vehicle_);
    filter_->setParams(service_ ? service_->params() : nullptr);
    // 기체가 바뀌면 같은 이름 목록이어도 처음 모습(모두 접힘, 전체 고름)으로 돌아가요.
    tree_->collapseAll();
    tree_->setNames(paramNames(service_ ? service_->params() : nullptr));
    setNodeKey(QStringLiteral("*"));
    setSelectedName({});
    if (service_) bindService();
    watchJob();
    emit paramCountChanged();
    emit pendingChanged();
    emit selectedChanged();
}
void ParamsController::bindService()
{
    const auto changed = [this] { emit stateChanged(); };
    connect(service_, &ParamService::downloadStateChanged, this, changed);
    connect(service_, &ParamService::metadataStateChanged, this, changed);
    connect(service_, &ParamService::rebootNeededChanged, this, changed);
    connect(service_, &ParamService::jobChanged, this, &ParamsController::watchJob);
    connect(service_, &ParamService::writeFinished, this, &ParamsController::writeDone);
    connect(service_, &ParamService::rebootFinished, this, [this](const kgc::app::errors::Result<void>& result) {
        emit toast(result ? text(QT_TRANSLATE_NOOP("Params", "재부팅 명령 전송됨")) : errorText(result.error()));
    });
    auto* params = service_->params();
    connect(params, &Params::pendingCountChanged, this, &ParamsController::pendingChanged);
    connect(params, &Params::groupsChanged, this, [this, params] {
        // 펼침은 남기고, 고른 마디가 없어졌으면 전체로, 남았으면 그 줄이 보이도록 조상을 펼쳐요.
        tree_->setNames(paramNames(params));
        if (!tree_->contains(nodeKey_)) setNodeKey(QStringLiteral("*"));
        else tree_->expandTo(nodeKey_);
    });
    connect(params, &QAbstractItemModel::dataChanged, this, &ParamsController::selectedChanged);
    connect(params, &QAbstractItemModel::rowsInserted, this, &ParamsController::paramCountChanged);
    connect(params, &QAbstractItemModel::rowsRemoved, this, &ParamsController::paramCountChanged);
}
void ParamsController::watchJob()
{
    if (auto* current = job()) {
        disconnect(current, &kgc::app::job::Job::finished, this, nullptr);
        connect(current, &kgc::app::job::Job::finished, this, [this, current] {
            if (current->state() != kgc::app::job::Job::State::Failed) return;
            if (current->errorCode() == make(ParamError::Armed) && current->kind() != kgc::app::job::OperationKind::Read)
                emit toast(text(QT_TRANSLATE_NOOP("Params", "%1개 쓰기 완료. 기체 시동으로 중단")).arg(lastWritten_));
            else emit toast(current->errorText());
        });
    }
    emit stateChanged();
}
void ParamsController::writeDone(int written, const QStringList& failed)
{
    lastWritten_ = written;
    if (failed.isEmpty()) {
        emit toast(text(QT_TRANSLATE_NOOP("Params", "%1개 쓰기 완료")).arg(written));
        return;
    }
    auto names = failed.first(std::min<qsizetype>(failed.size(), kToastNames)).join(QStringLiteral(", "));
    if (failed.size() > kToastNames)
        names += text(QT_TRANSLATE_NOOP("Params", " 외 %1개")).arg(failed.size() - kToastNames);
    emit toast(text(QT_TRANSLATE_NOOP("Params", "%1개 쓰기 완료. %2개 쓰기 실패: %3"))
                   .arg(written).arg(failed.size()).arg(names));
}
const Param* ParamsController::row(const QString& name) const { return service_ ? service_->params()->find(name) : nullptr; }
const ParamMeta* ParamsController::meta(const QString& name) const { return service_ ? service_->params()->meta(name) : nullptr; }
bool ParamsController::hasVehicle() const { return service_ != nullptr; }
QAbstractItemModel* ParamsController::model() const { return filter_; }
int ParamsController::visibleCount() const { return filter_->rowCount(); }
int ParamsController::paramCount() const { return service_ ? service_->params()->rowCount() : 0; }
int ParamsController::pendingCount() const { return service_ ? service_->params()->pendingCount() : 0; }
bool ParamsController::hasDefaults() const { return service_ && service_->params()->hasDefaults(); }
DownloadState ParamsController::downloadState() const { return service_ ? service_->downloadState() : DownloadState::Idle; }
MetadataState ParamsController::metadataState() const { return service_ ? service_->metadataState() : MetadataState::None; }
kgc::app::job::Job* ParamsController::job() const { return service_ ? service_->job() : nullptr; }
bool ParamsController::rebootNeeded() const { return service_ && service_->rebootNeeded(); }
int ParamsController::receivedCount() const { return service_ ? service_->receivedCount() : 0; }
int ParamsController::totalCount() const { return service_ ? service_->totalCount() : 0; }
QAbstractItemModel* ParamsController::tree() const { return tree_; }
QString ParamsController::nodeKey() const { return nodeKey_; }
void ParamsController::setNodeKey(const QString& key)
{
    if (nodeKey_ == key) return;
    nodeKey_ = key;
    filter_->setNodeKey(key);
    emit filterChanged();
}
void ParamsController::toggleNode(const QString& key)
{
    tree_->toggle(key);
    const bool branch = key.endsWith(QStringLiteral("_*")) && tree_->contains(key);
    // 고른 마디를 품은 가지를 접으면 고른 줄이 숨으니, 그 가지를 골라 표도 그 가지로 바꿔요.
    if (branch && !tree_->isExpanded(key) && paramNodeText(nodeKey_).startsWith(paramNodeText(key) + u'_'))
        setNodeKey(key);
}
QString ParamsController::nodeLabel(const QString& key, int depth) const
{
    const auto prefix = paramNodeText(key);
    if (!vehicle_ || depth != 0 || !key.endsWith(QStringLiteral("_*"))) return prefix;
    const auto title = vehicle_->plugin().paramGroupTitle(prefix);
    return title.isEmpty() ? prefix : QStringLiteral("%1 (%2)").arg(title, prefix);
}
QString ParamsController::searchText() const { return searchText_; }
void ParamsController::setSearchText(const QString& value)
{
    if (searchText_ == value) return;
    searchText_ = value;
    filter_->setTerms(searchTerms(value));
    emit filterChanged();
}
bool ParamsController::pendingOnly() const { return pendingOnly_; }
void ParamsController::setPendingOnly(bool value)
{
    if (pendingOnly_ == value) return;
    pendingOnly_ = value;
    filter_->setPendingOnly(value);
    emit filterChanged();
}
bool ParamsController::nonDefaultOnly() const { return nonDefaultOnly_; }
void ParamsController::setNonDefaultOnly(bool value)
{
    if (nonDefaultOnly_ == value) return;
    nonDefaultOnly_ = value;
    filter_->setNonDefaultOnly(value);
    emit filterChanged();
}
QString ParamsController::selectedName() const { return selectedName_; }
void ParamsController::setSelectedName(const QString& name)
{
    if (selectedName_ == name) return;
    selectedName_ = name;
    emit selectedChanged();
}
QVariantMap ParamsController::selected() const
{
    const auto* entry = row(selectedName_);
    if (!entry) return {};
    const auto* info = meta(selectedName_);
    const bool known = info && info->defaultValue;
    const bool restorable = known && !sameValue(*info->defaultValue, entry->value, entry->type)
        && !(entry->pending && sameValue(*info->defaultValue, *entry->pending, entry->type));
    return {{"name", entry->name}, {"fcText", formatValue(entry->value, entry->type)},
            {"pendingText", entry->pending ? formatValue(*entry->pending, entry->type) : QString{}},
            {"hasPending", entry->pending.has_value()}, {"canRestore", restorable},
            {"description", info ? (info->description.isEmpty() ? info->title : info->description) : QString{}}};
}
QVariantList ParamsController::diffRows() const
{
    QVariantList result;
    for (const auto& entry : diff_.changed) {
        const auto* current = row(entry.name);
        const auto type = current ? current->type : std::uint8_t{};
        result.append(QVariantMap{{"name", entry.name}, {"current", formatValue(entry.current, type)},
                                  {"file", formatValue(entry.fileValue, type)}});
    }
    return result;
}
QString ParamsController::diffSummary() const
{
    auto line = text(QT_TRANSLATE_NOOP("Params", "변경 %1 · 동일 %2 · FC에 없음 %3 · 읽기 전용 %4 · 건너뜀 %5"))
                    .arg(diff_.changed.size()).arg(diff_.same).arg(diff_.missing).arg(diff_.readOnly).arg(diff_.skipped);
    const int unread = diff_.badLines + diff_.otherComponent;
    if (unread > 0) line += text(QT_TRANSLATE_NOOP("Params", " · 읽지 못한 줄 %1")).arg(unread);
    return line;
}
QUrl ParamsController::saveFolder() const { return QUrl::fromLocalFile(kgc::app::paths::Paths::paramsBackups()); }
QUrl ParamsController::saveFileUrl() const
{
    if (!vehicle_) return {};
    const auto name = QStringLiteral("%1-%2.params").arg(vehicle_->states()->capabilities().firmwareName).arg(vehicle_->sysid());
    return QUrl::fromLocalFile(kgc::app::paths::Paths::paramsBackups() + QLatin1Char('/') + name);
}
QVariantMap ParamsController::checkValue(const QString& name, const QString& value) const
{
    const auto* entry = row(name);
    if (!entry) return {{"error", errorText(make(ParamError::NotFound))}};
    const auto shown = formatValue(entry->pending.value_or(entry->value), entry->type);
    if (value.trimmed() == shown) return {{"unchanged", true}};
    const auto parsed = parseValue(value, entry->type);
    if (!parsed) return {{"error", errorText(parsed.error())}};
    const auto* info = meta(name);
    const bool ask = outOfRange(*parsed, info);
    return {{"value", *parsed}, {"ask", ask}, {"range", ask ? formatRange(*info, entry->type) : QString{}}};
}
void ParamsController::setPending(const QString& name, double value)
{
    if (!service_) return;
    if (const auto result = service_->setPending(name, value); !result) emit toast(errorText(result.error()));
}
void ParamsController::revert(const QString& name)
{
    if (service_) service_->clearPending(name);
}
void ParamsController::restoreDefault(const QString& name)
{
    const auto* info = meta(name);
    if (info && info->defaultValue) setPending(name, *info->defaultValue);
}
QVariantMap ParamsController::optionModel(const QString& name) const
{
    const auto* entry = row(name);
    const auto* info = meta(name);
    if (!entry || !info) return {};
    const double current = entry->pending.value_or(entry->value);
    QVariantList items;
    int selected = -1;
    for (const auto& option : info->options) {
        if (sameValue(option.value, current, entry->type)) selected = items.size();
        items.append(QVariantMap{{"value", option.value}, {"text", option.label}});
    }
    if (selected < 0) {
        items.prepend(QVariantMap{{"value", current}, {"text", formatValue(current, entry->type)}});
        selected = 0;
    }
    return {{"items", items}, {"current", selected}};
}
QVariantList ParamsController::bitModel(const QString& name) const
{
    const auto* entry = row(name);
    const auto* info = meta(name);
    if (!entry || !info) return {};
    const double current = entry->pending.value_or(entry->value);
    QVariantList result;
    for (const auto& bit : info->bits) {
        if (bit.bit >= typeBits(entry->type)) continue;
        result.append(QVariantMap{{"bit", bit.bit}, {"label", bit.label}, {"checked", bitIsSet(current, entry->type, bit.bit)}});
    }
    return result;
}
double ParamsController::bitsValue(const QString& name, const QList<int>& chosen) const
{
    const auto* entry = row(name);
    const auto* info = meta(name);
    if (!entry || !info) return 0.0;
    QList<int> listed;
    for (const auto& bit : info->bits) listed.append(bit.bit);
    return withBits(entry->pending.value_or(entry->value), entry->type, listed, chosen);
}
QVariantList ParamsController::pendingRows() const
{
    QVariantList result;
    if (!service_) return result;
    for (const auto& entry : service_->params()->all())
        if (entry.pending)
            result.append(QVariantMap{{"name", entry.name}, {"current", formatValue(entry.value, entry.type)},
                                      {"next", formatValue(*entry.pending, entry.type)}});
    return result;
}
QString ParamsController::writeCheck()
{
    using kgc::vehicle::safety::Decision;
    if (!service_) return QStringLiteral("deny");
    const auto decision = service_->writeDecision();
    if (decision == Decision::Deny) {
        emit toast(errorText(make(ParamError::Armed)));
        return QStringLiteral("deny");
    }
    return decision == Decision::AllowWithWarning ? QStringLiteral("warn") : QStringLiteral("allow");
}
void ParamsController::write()
{
    if (service_) service_->writePending();
}
void ParamsController::refresh()
{
    if (service_) service_->refresh();
}
void ParamsController::reboot()
{
    if (service_) service_->reboot();
}
void ParamsController::saveFile(const QUrl& url)
{
    if (!service_) return;
    const auto result = service_->saveFile(url.toLocalFile());
    emit toast(result ? text(QT_TRANSLATE_NOOP("Params", "파일 저장 완료")) : errorText(result.error()));
}
bool ParamsController::openFile(const QUrl& url)
{
    if (!service_) return false;
    const auto result = service_->compareFile(url.toLocalFile());
    if (!result) {
        emit toast(errorText(result.error()));
        return false;
    }
    diff_ = *result;
    emit diffChanged();
    return true;
}
void ParamsController::applyFile(const QStringList& names)
{
    if (service_) service_->applyFile(names, diff_);
}
}
