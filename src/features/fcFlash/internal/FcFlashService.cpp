// [0.2.0 T14–T22] specs/features/0.2.0-fcflash.md
#include <kgc/fcFlash/FcFlashService.h>
namespace kgc::fcFlash {
struct FcFlashService::Private {
    kgc::comm::links::LinkManager& links;
    kgc::vehicle::registry::VehicleRegistry& vehicles;
    kgc::vehicle::safety::ArmedPolicy& policy;
    kgc::comm::rawport::PortRegistry& ports;
    kgc::app::net::Http& http;
    kgc::app::scheduler::Scheduler& scheduler;
    Step step = Step::Board;
    Phase phase = Phase::Idle;
    FirmwareSource source = FirmwareSource::Px4Official;
};
FcFlashService::FcFlashService(kgc::comm::links::LinkManager& links, kgc::vehicle::registry::VehicleRegistry& vehicles,
    kgc::vehicle::safety::ArmedPolicy& policy, kgc::comm::rawport::PortRegistry& ports,
    kgc::app::net::Http& http, kgc::app::scheduler::Scheduler& scheduler, QObject* parent)
    : QObject(parent), d_(new Private{links, vehicles, policy, ports, http, scheduler})
{
    // WORKER-FILL: spec §6.9 — 전용 QThread에 FlashWorker를 올리고 큐드 시그널을 이어요.
}
FcFlashService::~FcFlashService()
{
    // WORKER-FILL: spec §6.9 — 중단 플래그를 세우고 스레드를 wait해요.
}
Step FcFlashService::step() const { return d_->step; }
Phase FcFlashService::phase() const { return d_->phase; }
FirmwareSource FcFlashService::source() const { return d_->source; }
// WORKER-FILL: spec §6 — 아래 읽기 함수는 서비스가 든 값을 돌려주도록 Private에 값을 더해요.
int FcFlashService::secondsLeft() const { return 0; }
bool FcFlashService::canRetry() const { return false; }
QStringList FcFlashService::pickPorts() const { return {}; }
QString FcFlashService::boardManufacturer() const { return {}; }
QString FcFlashService::boardProduct() const { return {}; }
int FcFlashService::boardId() const { return 0; }
int FcFlashService::flashKb() const { return 0; }
int FcFlashService::bootloaderRevision() const { return 0; }
QString FcFlashService::vehicle() const { return {}; }
QStringList FcFlashService::versions() const { return {}; }
int FcFlashService::versionIndex() const { return -1; }
QStringList FcFlashService::candidates() const { return {}; }
int FcFlashService::candidateIndex() const { return -1; }
bool FcFlashService::listLoading() const { return false; }
bool FcFlashService::listFailed() const { return false; }
bool FcFlashService::canContinue() const { return false; }
QString FcFlashService::filePath() const { return {}; }
QString FcFlashService::firmwareName() const { return {}; }
QString FcFlashService::expectedVersion() const { return {}; }
QString FcFlashService::firmwareBoard() const { return {}; }
kgc::app::job::Job* FcFlashService::job() const { return nullptr; }
QString FcFlashService::vehicleVersion() const { return {}; }
QString FcFlashService::failureText() const
{
    // WORKER-FILL: spec §6.8·§7 — 오류 코드와 값으로 현재 언어의 실패 글자를 만들어요.
    return {};
}
void FcFlashService::beginProbe(const QString& portName)
{
    // WORKER-FILL: spec §6.3 — 1~10번 절차예요. known에 있던 다른 포트에는 바이트를 보내지 않아요.
    Q_UNUSED(portName);
}
void FcFlashService::pickPort(const QString& portName)
{
    // WORKER-FILL: spec §6.3-9 — 고른 포트로 Ready, 나머지는 부팅 없이 닫아요.
    Q_UNUSED(portName);
}
void FcFlashService::retryProbe()
{
    // WORKER-FILL: spec §6.3-10 — 마지막 포트로 beginProbe를 다시 해요.
}
void FcFlashService::chooseSource(FirmwareSource source)
{
    // WORKER-FILL: spec §6.5 — 출처별 목록을 받아 versions·candidates를 채워요.
    Q_UNUSED(source);
}
void FcFlashService::chooseVehicle(const QString& name)
{
    // WORKER-FILL: spec §6.5 — manifestVersions·manifestCandidates로 다시 채워요.
    Q_UNUSED(name);
}
void FcFlashService::chooseVersion(int index)
{
    // WORKER-FILL: spec §6.5 — 고른 버전의 후보를 다시 채우고 pickCandidate를 적용해요.
    Q_UNUSED(index);
}
void FcFlashService::chooseCandidate(int index)
{
    // WORKER-FILL: spec §6.5 — 고른 후보를 기억해요.
    Q_UNUSED(index);
}
void FcFlashService::chooseFile(const QString& path)
{
    // WORKER-FILL: spec §6.5·§6.6 — 작업 스레드에서 해석하고 checkImage로 호환을 봐요.
    Q_UNUSED(path);
}
void FcFlashService::toConfirm()
{
    // WORKER-FILL: spec §6.7 — 선택이 있으면 Step::Confirm으로 가요.
}
void FcFlashService::backToFirmware()
{
    // WORKER-FILL: spec §6.7 — Step::Firmware로 돌아가요.
}
void FcFlashService::startFlash()
{
    // WORKER-FILL: spec §6.8 — 1~10번 절차예요. 검증 실패에는 boot를 부르지 않아요.
}
void FcFlashService::cancelFlash()
{
    // WORKER-FILL: spec §6.8-3 — 지우기 전이면 finishCancelled, Phase::Ready, Step::Confirm이에요.
}
void FcFlashService::retryFlash()
{
    // WORKER-FILL: spec §6.8-5 — 내려받기 없이 sync·identify부터 다시 해요.
}
void FcFlashService::bootAndReset()
{
    // WORKER-FILL: spec §6.2·§6.5 — BOOT를 보내고 portClosed 뒤 임대를 놓고 Idle로 가요.
}
void FcFlashService::reset()
{
    // WORKER-FILL: spec §6.2·§6.8-5 — 부팅 없이 포트를 닫고 portClosed 뒤 임대를 놓아요.
}
}
