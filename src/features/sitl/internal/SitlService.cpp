// [0.4.0 5.4 6.1–6.7 6.9] specs/features/0.4.0-sitl.md
#include <kgc/sitl/SitlService.h>
#include <kgc/sitl/Errors.h>
#include <kgc/sitl/Settings.h>
#include <kgc/app/job/Errors.h>
#include <kgc/app/job/Job.h>
#include <kgc/app/log/Log.h>
#include <kgc/app/paths/Paths.h>
#include <kgc/app/scheduler/Timeout.h>
#include <kgc/comm/links/Errors.h>
#include <kgc/comm/links/LinkManager.h>
#include "Constants.h"
#include "Events.h"
#include "Simulator.h"
#include "SitlCommand.h"
#include "SitlDownload.h"
#include "SitlSources.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPointer>
namespace kgc::sitl {
using kgc::app::errors::ErrorCode;
using kgc::app::errors::Result;
using kgc::app::paths::Paths;
using kgc::comm::links::LinkConfig;
namespace {
// 운영체제는 이 한 곳에서 정해요(스펙 §6.1).
constexpr SitlPlatform currentPlatform()
{
#ifdef Q_OS_WIN
    return SitlPlatform::Windows;
#else
    return SitlPlatform::Linux;
#endif
}
// 로그는 영어만 쓰므로 링크 오류는 열거 이름으로, 다른 모듈의 코드는 번호로 적어요.
QString linkErrorName(ErrorCode error)
{
    using kgc::comm::links::LinkError;
    if (error.module == kgc::comm::links::kModuleId) {
        switch (static_cast<LinkError>(error.code)) {
        case LinkError::AlreadyConnected: return QStringLiteral("AlreadyConnected");
        case LinkError::PortOpenFailed: return QStringLiteral("PortOpenFailed");
        case LinkError::PermissionDenied: return QStringLiteral("PermissionDenied");
        case LinkError::PortBusy: return QStringLiteral("PortBusy");
        case LinkError::UdpBindFailed: return QStringLiteral("UdpBindFailed");
        case LinkError::PortLost: return QStringLiteral("PortLost");
        }
    }
    return QStringLiteral("%1:%2").arg(error.module.value).arg(error.code);
}
QString boolText(bool value) { return value ? QStringLiteral("true") : QStringLiteral("false"); }
QDir cacheDir(SitlVehicle vehicle, SitlRelease release)
{
    return QDir(QDir(Paths::cacheSitl()).filePath(sitlFolderName(currentPlatform(), vehicle, release)));
}
}
struct SitlService::Private {
    Private(kgc::comm::links::LinkManager& links, kgc::app::net::Http& http) : links(links), http(http) {}
    kgc::comm::links::LinkManager& links;
    kgc::app::net::Http& http;
    SitlState state = SitlState::Idle;
    std::optional<ErrorCode> lastError;
    int lastExitCode = -1;
    QStringList output;
    kgc::app::scheduler::Timeout* outputThrottle = nullptr;
    bool outputPending = false;
    std::unique_ptr<Simulator> simulator;
    QPointer<SitlDownload> download;
    SitlVehicle vehicle = SitlVehicle::Copter;
    SitlRelease release = SitlRelease::Stable;
    SitlHome home{};
    int speedup = 1;
    bool wipe = false;
    // 이 서비스가 요청했거나 넘겨받은 링크 번호예요. 열리기 전에도 번호가 있어요.
    int sitlLinkId = -1;
    bool sitlLinkOpen = false;
    bool reportedBusy = false;
    bool reportedOpen = false;
    bool working() const
    {
        return state == SitlState::Downloading || state == SitlState::Starting || state == SitlState::Running;
    }
    QString folderName() const { return sitlFolderName(currentPlatform(), vehicle, release); }
};
SitlService::SitlService(kgc::comm::links::LinkManager& links, kgc::app::net::Http& http, QObject* parent)
    : QObject(parent), d_(std::make_unique<Private>(links, http))
{
    d_->simulator = std::make_unique<Simulator>();
    d_->outputThrottle = new kgc::app::scheduler::Timeout(QStringLiteral("sitlOutput"), this);
    connect(d_->outputThrottle, &kgc::app::scheduler::Timeout::expired, this, [this] {
        d_->outputPending = false;
        emit outputChanged();
    });
    wireLinks();
    wireSimulator();
    if (auto* app = QCoreApplication::instance()) connect(app, &QCoreApplication::aboutToQuit, this, &SitlService::stop);
}
SitlService::~SitlService()
{
    // LinkManager는 부르지 않고 프로세스만 멈춰요. 멈추며 나는 시그널은 받지 않아요.
    d_->simulator->disconnect(this);
    d_->simulator.reset();
    delete d_->download.data();
}
SitlState SitlService::state() const { return d_->state; }
kgc::app::job::Job* SitlService::job() const { return d_->download ? d_->download->job() : nullptr; }
bool SitlService::cached(SitlVehicle vehicle, SitlRelease release) const
{
    const auto files = sitlFiles(currentPlatform(), vehicle, release);
    const auto dir = cacheDir(vehicle, release);
    return QFileInfo::exists(dir.filePath(files.first().fileName)) && QFileInfo::exists(dir.filePath(files.last().fileName));
}
QDateTime SitlService::cachedAt(SitlVehicle vehicle, SitlRelease release) const
{
    const QFileInfo executable(cacheDir(vehicle, release).filePath(sitlFiles(currentPlatform(), vehicle, release).first().fileName));
    return executable.exists() ? executable.lastModified() : QDateTime{};
}
bool SitlService::linkOpenedBySitl() const { return d_->sitlLinkOpen; }
bool SitlService::linkBusy() const { return d_->links.linkId() >= 0 && !linkOpenedBySitl(); }
int SitlService::lastExitCode() const { return d_->lastExitCode; }
QStringList SitlService::output() const { return d_->output; }
std::optional<ErrorCode> SitlService::lastError() const { return d_->lastError; }
Result<void> SitlService::start(SitlVehicle vehicle, SitlRelease release, const SitlHome& home, int speedup, bool wipe)
{
    if (d_->working()) return std::unexpected(make(SitlError::Busy));
    if (!checkHome(home).isEmpty() || speedup < settings::speedupMin() || speedup > settings::speedupMax())
        return std::unexpected(make(SitlError::BadHome));
    if (d_->links.linkId() >= 0) return std::unexpected(make(SitlError::LinkBusy));
    settings::setVehicle(sitlVehicleToSetting(vehicle));
    settings::setRelease(sitlReleaseToSetting(release));
    settings::setHomeLatitude(home.latitude);
    settings::setHomeLongitude(home.longitude);
    settings::setHomeAltitude(home.altitude);
    settings::setHomeHeading(home.heading);
    settings::setSpeedup(speedup);
    d_->vehicle = vehicle;
    d_->release = release;
    d_->home = home;
    d_->speedup = speedup;
    d_->wipe = wipe;
    if (cached(vehicle, release)) launch();
    else beginDownload(vehicle, release, true);
    return {};
}
void SitlService::stop()
{
    if (d_->state == SitlState::Downloading) {
        if (d_->download) d_->download->job()->cancel();
        return;
    }
    if (d_->state != SitlState::Starting && d_->state != SitlState::Running) return;
    if (d_->sitlLinkId >= 0 && d_->links.linkId() == d_->sitlLinkId) d_->links.disconnectLink();
    d_->simulator->stop();
    setState(SitlState::Idle);
}
Result<void> SitlService::redownload(SitlVehicle vehicle, SitlRelease release)
{
    if (d_->working()) return std::unexpected(make(SitlError::Busy));
    beginDownload(vehicle, release, false);
    return {};
}
Result<void> SitlService::reconnect()
{
    if (d_->state != SitlState::Running) return std::unexpected(make(SitlError::Busy));
    if (d_->links.linkId() >= 0) return std::unexpected(make(SitlError::LinkBusy));
    const auto result = d_->links.connectLink({LinkConfig::Kind::Udp, {}, 0});
    if (!result) return std::unexpected(make(SitlError::LinkBusy));
    d_->sitlLinkId = d_->links.linkId();
    refreshLinkFlags();
    return {};
}
void SitlService::beginDownload(SitlVehicle vehicle, SitlRelease release, bool launchAfter)
{
    const auto platform = currentPlatform();
    d_->download = new SitlDownload(d_->http, sitlFiles(platform, vehicle, release), Paths::cacheSitl(),
                                    sitlFolderName(platform, vehicle, release), platform == SitlPlatform::Linux, this);
    connect(d_->download, &SitlDownload::finished, this, [this, launchAfter](const Result<void>& result) {
        downloadFinished(result, launchAfter);
    });
    setState(SitlState::Downloading);
    emit jobChanged();
    d_->download->start();
}
void SitlService::downloadFinished(const Result<void>& result, bool launchAfter)
{
    if (d_->download) d_->download->deleteLater();
    d_->download = nullptr;
    emit jobChanged();
    if (result) {
        emit cacheChanged();
        if (launchAfter) launch();
        else setState(SitlState::Idle);
    } else if (result.error() == make(kgc::app::job::JobError::Cancelled)) {
        setState(SitlState::Idle);
    } else {
        setState(SitlState::Failed, make(SitlError::DownloadFailed));
    }
}
void SitlService::launch()
{
    const auto files = sitlFiles(currentPlatform(), d_->vehicle, d_->release);
    const auto cache = cacheDir(d_->vehicle, d_->release);
    const auto runRoot = Paths::sitl();
    const auto runDir = QDir(runRoot).filePath(sitlRunFolderName(d_->vehicle));
    const auto defaults = QDir(runDir).filePath(QString::fromLatin1(kDefaultsFile));
    // 기본 파라미터는 실행 폴더의 defaults.parm으로 늘 덮어써요.
    if (runRoot.isEmpty() || !QDir().mkpath(runDir) || (QFile::exists(defaults) && !QFile::remove(defaults))
        || !QFile::copy(cache.filePath(files.last().fileName), defaults)) {
        KGC_LOG_WARN(kCategory, SitlEvent::SitlStartFailed, "SITL start failed: %1", QStringLiteral("Defaults copy failed"));
        setState(SitlState::Failed, make(SitlError::StartFailed));
        return;
    }
    d_->output.clear();
    emit outputChanged();
    d_->lastExitCode = -1;
    setState(SitlState::Starting);
    d_->simulator->start(cache.absoluteFilePath(files.first().fileName),
                         sitlArguments(d_->vehicle, d_->home, d_->speedup, d_->wipe), runDir);
}
void SitlService::connectSitlLink()
{
    const auto result = d_->links.connectLink({LinkConfig::Kind::Udp, {}, 0});
    if (!result) {
        KGC_LOG_WARN(kCategory, SitlEvent::SitlLinkFailed, "SITL link failed: %1", linkErrorName(result.error()));
        d_->simulator->stop();
        setState(SitlState::Failed, make(SitlError::LinkBusy));
        return;
    }
    d_->sitlLinkId = d_->links.linkId();
    refreshLinkFlags();
}
void SitlService::wireLinks()
{
    using kgc::comm::links::LinkManager;
    connect(&d_->links, &LinkManager::linkOpened, this, &SitlService::onLinkOpened);
    connect(&d_->links, &LinkManager::linkFailed, this, &SitlService::onLinkFailed);
    connect(&d_->links, &LinkManager::linkClosed, this, [this](int linkId, std::optional<ErrorCode>) { onLinkClosed(linkId); });
}
void SitlService::wireSimulator()
{
    auto* simulator = d_->simulator.get();
    connect(simulator, &Simulator::started, this, [this](qint64 pid) {
        KGC_LOG_INFO(kCategory, SitlEvent::SitlStarted, "SITL started: %1, pid %2", d_->folderName(), pid);
        if (d_->state == SitlState::Starting) connectSitlLink();
    });
    connect(simulator, &Simulator::failedToStart, this, [this](const QString& reason) {
        KGC_LOG_WARN(kCategory, SitlEvent::SitlStartFailed, "SITL start failed: %1", reason);
        if (d_->state == SitlState::Starting) setState(SitlState::Failed, make(SitlError::StartFailed));
    });
    connect(simulator, &Simulator::exited, this, &SitlService::onExited);
    connect(simulator, &Simulator::line, this, &SitlService::onLine);
}
void SitlService::onLinkOpened(int linkId)
{
    const bool udp = d_->links.linkConfig().kind == LinkConfig::Kind::Udp;
    // 실행 중에 상단 연결 단추가 연 UDP 링크도 이 서비스의 것으로 봐요(스펙 §6.5).
    if (linkId == d_->sitlLinkId || (d_->state == SitlState::Running && udp)) {
        d_->sitlLinkId = linkId;
        d_->sitlLinkOpen = true;
        KGC_LOG_INFO(kCategory, SitlEvent::SitlLinkOpened, "SITL link opened: UDP %1", kgc::comm::links::kUdpPort);
        if (d_->state == SitlState::Starting) setState(SitlState::Running);
    }
    refreshLinkFlags();
}
void SitlService::onLinkFailed(ErrorCode error)
{
    // 링크는 하나뿐이라, 이 서비스가 요청한 링크가 아직 열리지 않았으면 그 실패예요.
    if (d_->sitlLinkId >= 0 && !d_->sitlLinkOpen) {
        KGC_LOG_WARN(kCategory, SitlEvent::SitlLinkFailed, "SITL link failed: %1", linkErrorName(error));
        d_->sitlLinkId = -1;
        if (d_->state == SitlState::Starting) {
            d_->simulator->stop();
            setState(SitlState::Failed, make(SitlError::LinkFailed));
        }
    }
    refreshLinkFlags();
}
void SitlService::onLinkClosed(int linkId)
{
    if (linkId == d_->sitlLinkId) {
        d_->sitlLinkId = -1;
        d_->sitlLinkOpen = false;
        // 열리기 전에 다른 곳에서 끊었으면 프로세스는 두고 다시 연결할 수 있게 해요.
        if (d_->state == SitlState::Starting) setState(SitlState::Running);
    }
    refreshLinkFlags();
}
void SitlService::onExited(int exitCode, bool crashed, bool requested)
{
    KGC_LOG_INFO(kCategory, SitlEvent::SitlExited, "SITL exited: code %1, crashed %2, requested %3",
                 exitCode, boolText(crashed), boolText(requested));
    if (requested || (d_->state != SitlState::Starting && d_->state != SitlState::Running)) return;
    if (d_->sitlLinkId >= 0 && d_->links.linkId() == d_->sitlLinkId) d_->links.disconnectLink();
    d_->lastExitCode = crashed ? -1 : exitCode;
    setState(SitlState::Failed, make(SitlError::Crashed));
}
void SitlService::onLine(QString text)
{
    if (text.endsWith(QLatin1Char('\r'))) text.chop(1);
    d_->output.append(text);
    if (d_->output.size() > kSitlOutputLines) d_->output.remove(0, d_->output.size() - kSitlOutputLines);
    if (d_->outputPending) return;
    d_->outputPending = true;
    d_->outputThrottle->start(kSitlOutputThrottle);
}
void SitlService::setState(SitlState state, std::optional<ErrorCode> error)
{
    d_->state = state;
    d_->lastError = state == SitlState::Failed ? error : std::nullopt;
    emit stateChanged();
    refreshLinkFlags();
}
void SitlService::refreshLinkFlags()
{
    if (const bool busy = linkBusy(); busy != d_->reportedBusy) {
        d_->reportedBusy = busy;
        emit linkBusyChanged();
    }
    if (const bool open = linkOpenedBySitl(); open != d_->reportedOpen) {
        d_->reportedOpen = open;
        emit linkOpenedBySitlChanged();
    }
}
}
