// [0.2.0 T14–T22] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/job/Job.h>
#include <kgc/fcFlash/FlashStates.h>
#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>
namespace kgc::app::net { class Http; }
namespace kgc::app::scheduler { class Scheduler; }
namespace kgc::comm::links { class LinkManager; }
namespace kgc::comm::rawport { class PortRegistry; }
namespace kgc::vehicle::registry { class VehicleRegistry; }
namespace kgc::vehicle::safety { class ArmedPolicy; }
namespace kgc::fcFlash {
/** 보드 확인부터 재연결까지 펌웨어 굽기 흐름을 GUI 스레드에서 맡아요.
 * 입력: 스펙 §5.10의 전역 객체들과 화면의 공개 함수 호출이에요.
 * 출력: 스펙 §6의 값을 프로퍼티로 내놓아요. 값이 바뀌면 changed를 내요.
 * 실패: 실패는 Phase::Failed, canRetry, failureText로 내놓아요.
 */
class FcFlashService : public QObject {
    Q_OBJECT
    Q_PROPERTY(kgc::fcFlash::Step step READ step NOTIFY changed)
    Q_PROPERTY(kgc::fcFlash::Phase phase READ phase NOTIFY changed)
    Q_PROPERTY(int secondsLeft READ secondsLeft NOTIFY changed)
    Q_PROPERTY(bool canRetry READ canRetry NOTIFY changed)
    Q_PROPERTY(QStringList pickPorts READ pickPorts NOTIFY changed)
    Q_PROPERTY(QString boardManufacturer READ boardManufacturer NOTIFY changed)
    Q_PROPERTY(QString boardProduct READ boardProduct NOTIFY changed)
    Q_PROPERTY(int boardId READ boardId NOTIFY changed)
    Q_PROPERTY(int flashKb READ flashKb NOTIFY changed)
    Q_PROPERTY(int bootloaderRevision READ bootloaderRevision NOTIFY changed)
    Q_PROPERTY(kgc::fcFlash::FirmwareSource source READ source NOTIFY changed)
    Q_PROPERTY(QString vehicle READ vehicle NOTIFY changed)
    Q_PROPERTY(QStringList versions READ versions NOTIFY changed)
    Q_PROPERTY(int versionIndex READ versionIndex NOTIFY changed)
    Q_PROPERTY(QStringList candidates READ candidates NOTIFY changed)
    Q_PROPERTY(int candidateIndex READ candidateIndex NOTIFY changed)
    Q_PROPERTY(bool listLoading READ listLoading NOTIFY changed)
    Q_PROPERTY(bool listFailed READ listFailed NOTIFY changed)
    Q_PROPERTY(bool canContinue READ canContinue NOTIFY changed)
    Q_PROPERTY(QString filePath READ filePath NOTIFY changed)
    Q_PROPERTY(QString firmwareName READ firmwareName NOTIFY changed)
    Q_PROPERTY(QString expectedVersion READ expectedVersion NOTIFY changed)
    Q_PROPERTY(QString firmwareBoard READ firmwareBoard NOTIFY changed)
    Q_PROPERTY(kgc::app::job::Job* job READ job NOTIFY changed)
    Q_PROPERTY(QString vehicleVersion READ vehicleVersion NOTIFY changed)
public:
/** 서비스를 대기 상태로 만들어요.
 * 입력: 수명 동안 유지되는 스펙 §5.10의 전역 객체들과 선택적인 parent예요.
 * 출력: Step::Board, Phase::Idle 상태의 서비스예요. 전용 스레드에 FlashWorker를 올려요.
 * 실패: 없어요.
 */
    FcFlashService(kgc::comm::links::LinkManager& links, kgc::vehicle::registry::VehicleRegistry& vehicles,
                   kgc::vehicle::safety::ArmedPolicy& policy, kgc::comm::rawport::PortRegistry& ports,
                   kgc::app::net::Http& http, kgc::app::scheduler::Scheduler& scheduler, QObject* parent = nullptr);
/** 중단 플래그를 세우고 작업 스레드가 끝나길 기다려요.
 * 입력: 없어요.
 * 출력: 스레드를 wait하고 정리해요. 앱 종료를 막지 않아요.
 * 실패: 없어요.
 */
    ~FcFlashService() override;
    /** 입력: 없어요. 출력: 지금 화면 단계예요. 실패: 없어요. */
    Step step() const;
    /** 입력: 없어요. 출력: 지금 Phase예요. 실패: 없어요. */
    Phase phase() const;
    /** 입력: 없어요. 출력: WaitReplug의 남은 초이며, 그 밖에는 0이에요. 실패: 없어요. */
    int secondsLeft() const;
    /** 입력: 없어요. 출력: Failed에서 다시 할 수 있으면 참이에요. 실패: 없어요. */
    bool canRetry() const;
    /** 입력: 없어요. 출력: PickPort에서 응답한 포트 이름 목록이에요. 실패: 없어요. */
    QStringList pickPorts() const;
    /** 입력: 없어요. 출력: USB 제조사 이름이며, 없으면 빈 값이에요. 실패: 없어요. */
    QString boardManufacturer() const;
    /** 입력: 없어요. 출력: USB 제품 이름이며, 없으면 빈 값이에요. 실패: 없어요. */
    QString boardProduct() const;
    /** 입력: 없어요. 출력: 부트로더가 알려 준 보드 ID이며, 모르면 0이에요. 실패: 없어요. */
    int boardId() const;
    /** 입력: 없어요. 출력: 플래시 크기(KB)이며, 모르면 0이에요. 실패: 없어요. */
    int flashKb() const;
    /** 입력: 없어요. 출력: 부트로더 판이며, 모르면 0이에요. 실패: 없어요. */
    int bootloaderRevision() const;
    /** 입력: 없어요. 출력: 고른 출처이며, 기본은 Px4Official이에요. 실패: 없어요. */
    FirmwareSource source() const;
    /** 입력: 없어요. 출력: ArduPilot 기체 이름(Copter·Heli·Plane·Rover·Sub)이에요. 실패: 없어요. */
    QString vehicle() const;
    /** 입력: 없어요. 출력: 버전 콤보의 값 목록이며, 첫 항목이 안정판이에요. 실패: 없어요. */
    QStringList versions() const;
    /** 입력: 없어요. 출력: 고른 버전 위치이며, 없으면 -1이에요. 실패: 없어요. */
    int versionIndex() const;
    /** 입력: 없어요. 출력: 후보 표시 목록이에요. 실패: 없어요. */
    QStringList candidates() const;
    /** 입력: 없어요. 출력: 고른 후보 위치이며, 없으면 -1이에요. 실패: 없어요. */
    int candidateIndex() const;
    /** 입력: 없어요. 출력: 목록을 받거나 해석하는 중이면 참이에요. 실패: 없어요. */
    bool listLoading() const;
    /** 입력: 없어요. 출력: 목록을 받지 못했으면 참이에요. 실패: 없어요. */
    bool listFailed() const;
    /** 입력: 없어요. 출력: 다음 단추를 쓸 수 있으면(선택이 있으면) 참이에요. 실패: 없어요. */
    bool canContinue() const;
    /** 입력: 없어요. 출력: 고른 파일 경로이며, 없으면 빈 값이에요. 실패: 없어요. */
    QString filePath() const;
    /** 입력: 없어요. 출력: 올릴 펌웨어 종류 이름이에요. 실패: 없어요. */
    QString firmwareName() const;
    /** 입력: 없어요. 출력: 비교 버전 expectedVersion이며, 모르면 빈 값이에요. 실패: 없어요. */
    QString expectedVersion() const;
    /** 입력: 없어요. 출력: 공식은 후보 표시, 파일은 summary예요. 실패: 없어요. */
    QString firmwareBoard() const;
    /** 입력: 없어요. 출력: 이번 굽기의 Job이며, 없으면 nullptr예요. 소유권은 넘기지 않아요. 실패: 없어요. */
    kgc::app::job::Job* job() const;
    /** 입력: 없어요. 출력: 재연결한 기체의 버전이며, 모르면 빈 값이에요. 실패: 없어요. */
    QString vehicleVersion() const;
/** 실패 글자를 현재 언어로 만들어요.
 * 입력: 없어요.
 * 출력: 스펙 §6·§7의 문구이며, WrongBoard는 서비스가 든 두 값을 채워요. 실패가 없으면 빈 값이에요.
 * 실패: 없어요.
 */
    QString failureText() const;
/** 고른 포트로 보드 확인을 시작해요.
 * 입력: 포트 이름 portName이에요.
 * 출력: Phase가 Probing을 지나 Ready, WaitReplug, PickPort 또는 Failed로 가요.
 * 실패: 시동 중이면 Armed, 다른 링크가 있으면 OtherLink로 Failed가 돼요.
 * 흐름:
 * 1. ArmedPolicy::check(FcFlash, Any) — Deny면 시작하지 않아요.
 * 2. LinkManager::refreshPorts — 포트 목록을 known으로 기억해요.
 * 3. PortLease::acquire → FlashWorker::probePorts — 고른 포트에 sync·identify를 해요. 성공하면 Ready예요.
 * 4. LinkManager::connectLink — 없으면 임시 링크를 열고 kHeartbeatWait 동안 기체를 기다려요.
 * 5. CommandQueue::send — 재부팅 명령(param1 = 3)을 보내요. ACCEPTED면 끊고 Rebooting이에요.
 * 6. Scheduler fcFlashScan — 500ms마다 후보 포트를 탐색해요. 실패하면 WaitReplug, 시간이 넘으면 BootloaderNotFound예요.
 */
    void beginProbe(const QString& portName);
/** PickPort에서 포트 하나를 골라요.
 * 입력: 포트 이름 portName이에요.
 * 출력: 고른 포트를 쥐고 Ready로 가요. 나머지 포트는 부팅 없이 닫아요.
 * 실패: PickPort가 아니면 무시해요.
 * 흐름:
 * 1. FlashWorker::keepPort — 고른 포트만 남기고 닫아요.
 */
    void pickPort(const QString& portName);
/** 실패한 보드 확인을 처음부터 다시 해요.
 * 입력: 없어요.
 * 출력: 마지막으로 고른 포트로 beginProbe를 다시 해요.
 * 실패: canRetry가 거짓이면 무시해요.
 */
    void retryProbe();
/** 펌웨어 출처를 골라요.
 * 입력: 출처 source예요.
 * 출력: 공식 출처면 목록을 받아 versions와 candidates를 채워요.
 * 실패: 목록을 못 받으면 listFailed가 참이에요.
 * 흐름:
 * 1. Http::get — PX4 릴리스 또는 ArduPilot 목록을 받아요.
 * 2. FlashWorker::parseManifest — ArduPilot 목록은 작업 스레드에서 풀고 해석해요.
 * 3. pickCandidate — 후보가 하나면 자동으로 골라요.
 */
    void chooseSource(FirmwareSource source);
/** ArduPilot 기체를 골라요.
 * 입력: 기체 이름 name이에요.
 * 출력: versions와 candidates를 다시 채워요.
 * 실패: 없어요.
 */
    void chooseVehicle(const QString& name);
/** 버전을 골라요.
 * 입력: versions의 위치 index예요.
 * 출력: candidates를 다시 채워요.
 * 실패: 범위 밖이면 무시해요.
 */
    void chooseVersion(int index);
/** 후보를 골라요.
 * 입력: candidates의 위치 index예요.
 * 출력: candidateIndex를 바꿔요.
 * 실패: 범위 밖이면 무시해요.
 */
    void chooseCandidate(int index);
/** 파일을 골라 바로 해석하고 호환을 봐요.
 * 입력: .px4 또는 .apj 파일 경로 path예요.
 * 출력: 맞으면 filePath와 canContinue를 채워요.
 * 실패: BadImage, WrongBoard, TooLarge를 failureText로 내놓아요.
 * 흐름:
 * 1. FlashWorker::parseImage — 작업 스레드에서 파일을 해석해요.
 * 2. checkImage — 보드 ID와 크기를 봐요.
 */
    void chooseFile(const QString& path);
/** 확인 단계로 가요.
 * 입력: 없어요.
 * 출력: Step::Confirm이에요.
 * 실패: canContinue가 거짓이면 무시해요.
 */
    void toConfirm();
/** 펌웨어 선택 단계로 돌아가요.
 * 입력: 없어요.
 * 출력: Step::Firmware예요.
 * 실패: 없어요.
 */
    void backToFirmware();
/** 굽기를 시작해요.
 * 입력: 없어요.
 * 출력: 새 Job을 만들고 Flashing으로 가요. 검증이 성공하면 부팅하고 Reconnecting으로 가요.
 * 실패: 실패하면 Job fail, Phase::Failed예요. 검증 실패에는 부팅하지 않아요.
 * 흐름:
 * 1. Http::download — 공식 출처만 Paths::cacheFirmware로 내려받아요.
 * 2. FlashWorker::flash — 해석, 호환 판정, Bootloader::flashImage를 해요.
 * 3. Job::succeed → FlashWorker::bootAndClose — 부팅하고 포트를 닫아요.
 * 4. portClosed → PortLease 해제 → LinkManager::connectLink — 다시 연결해요.
 * 5. Capabilities 묶음 — 종류와 버전을 비교해 Done, Mismatch, NoReturn으로 끝내요.
 */
    void startFlash();
/** 지우기 전의 굽기를 취소해요.
 * 입력: 없어요.
 * 출력: Job::finishCancelled, Phase::Ready, Step::Confirm이에요. 임대는 유지해요.
 * 실패: cancellable이 거짓이면 무시해요.
 */
    void cancelFlash();
/** 실패한 굽기를 내려받기 없이 다시 해요.
 * 입력: 없어요.
 * 출력: 새 Job으로 sync·identify부터 다시 해요.
 * 실패: canRetry가 거짓이면 무시해요.
 */
    void retryFlash();
/** 굽지 않고 FC를 부팅시키고 처음으로 돌아가요.
 * 입력: 없어요.
 * 출력: BOOT를 보내고 포트를 닫은 뒤 임대를 놓고 Idle로 가요.
 * 실패: 없어요.
 */
    void bootAndReset();
/** 부팅 없이 처음으로 돌아가요.
 * 입력: 없어요.
 * 출력: 포트를 닫고 임대를 놓은 뒤 Idle로 가요. 연결이 있으면 유지해요.
 * 실패: 없어요.
 */
    void reset();
signals:
/** 프로퍼티를 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: 변경 알림이에요.
 * 실패: 없어요.
 */
    void changed();
private:
    struct Private;
    std::unique_ptr<Private> d_;
};
}
