// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#include <kgc/comm/links/LinkManager.h>
#include <kgc/comm/links/UdpLink.h>
#include <kgc/comm/links/SerialLink.h>
#include <kgc/comm/links/Errors.h>
#include <kgc/app/scheduler/Scheduler.h>
#include <kgc/app/log/Log.h>
#include "LinkManagerPrivate.h"
#include "Constants.h"
#include "Events.h"
namespace kgc::comm::links {
LinkManager::LinkManager(kgc::app::scheduler::Scheduler& scheduler, QString extraSerial, QObject* parent)
    : QObject(parent), d_(std::make_unique<Private>())
{
    qRegisterMetaType<mavlink_message_t>();
    qRegisterMetaType<kgc::app::errors::ErrorCode>();
    qRegisterMetaType<std::optional<kgc::app::errors::ErrorCode>>();
    d_->extraSerial = std::move(extraSerial);
    refreshPorts();
    scheduler.registerTask("serialPorts", kPortListPeriod, this, this, [this] { refreshPorts(); });
}
LinkManager::~LinkManager()
{
    if (!d_->thread) return;
    if (d_->thread->isRunning()) {
        // 종료 중인 이벤트 루프에 동기 호출을 걸면 기다림이 끝나지 않을 수 있어요.
        // finished에 연결한 지연 삭제가 워커 스레드에서 포트도 함께 닫아요.
        d_->thread->quit();
        d_->thread->wait();
    }
}
kgc::app::errors::Result<void> LinkManager::connectLink(const LinkConfig& config)
{
    if (d_->thread) return std::unexpected(make(LinkError::AlreadyConnected));
    d_->config = config;
    d_->id = ++d_->nextId;
    d_->open = d_->failed = d_->closing = d_->cancelled = false;
    d_->error.reset();
    d_->thread = new QThread(this);
    d_->worker = config.kind == LinkConfig::Kind::Udp ? static_cast<Link*>(new UdpLink(config)) : new SerialLink(config);
    auto* worker = d_->worker.data();
    worker->moveToThread(d_->thread);
    connect(d_->thread, &QThread::started, worker, [worker] { worker->open(); });
    connect(d_->thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &Link::opened, this, [this] {
        if (d_->closing) return;
        d_->open = true;
        emit linkOpened(d_->id);
    }, Qt::QueuedConnection);
    connect(worker, &Link::messageReceived, this, [this](const QVariant& message) {
        if (d_->open && !d_->closing) emit messageReceived(d_->id, message.value<mavlink_message_t>());
    }, Qt::QueuedConnection);
    connect(worker, &Link::failed, this, [this](kgc::app::errors::ErrorCode error) {
        if (!d_->cancelled) {
            d_->failed = true;
            d_->error = error;
        }
        d_->closing = true;
        d_->open = false;
        d_->thread->quit();
    }, Qt::QueuedConnection);
    connect(worker, &Link::closed, this, [this](std::optional<kgc::app::errors::ErrorCode> error) {
        if (!d_->cancelled && error) d_->error = error;
        d_->closing = true;
        d_->open = false;
        d_->thread->quit();
    }, Qt::QueuedConnection);
    connect(d_->thread, &QThread::finished, this, [this] {
        const auto id = d_->id;
        const auto error = d_->error;
        const auto failed = d_->failed;
        d_->thread->deleteLater();
        d_->thread = nullptr;
        d_->worker = nullptr;
        d_->id = -1;
        d_->open = false;
        KGC_LOG_INFO(kCategory, LinksEvent::LinkWorkerStopped, "Link worker stopped: %1", id);
        if (failed) emit linkFailed(*error);
        else emit linkClosed(id, error);
    });
    d_->thread->start();
    return {};
}
void LinkManager::disconnectLink()
{
    if (!d_->thread) return;
    d_->cancelled = true;
    d_->failed = false;
    d_->error.reset();
    if (d_->closing || !d_->worker) return;
    d_->closing = true;
    QMetaObject::invokeMethod(d_->worker, [worker = d_->worker] { if (worker) worker->close(); }, Qt::QueuedConnection);
}
bool LinkManager::writeBytes(int linkId, const QByteArray& bytes)
{
    if (!d_->open || d_->closing || d_->id != linkId || !d_->worker) return false;
    return QMetaObject::invokeMethod(d_->worker, [worker = d_->worker, bytes] {
        if (worker) worker->writeBytes(bytes);
    }, Qt::QueuedConnection);
}
LinkConfig LinkManager::linkConfig() const { return d_->config; }
int LinkManager::linkId() const { return d_->id; }
std::uint8_t LinkManager::channel() const { return kLinkChannel; }
QList<QPair<QString, QString>> LinkManager::ports() const { return d_->ports; }
}
