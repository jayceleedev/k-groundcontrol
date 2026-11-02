// [0.3.0 5.5 6.8-1] specs/features/0.3.0-params.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <kgc/app/job/Job.h>
#include <kgc/params/ParamStates.h>
#include <kgc/params/Params.h>
#include <kgc/vehicle/registry/Vehicle.h>
#include <kgc/vehicle/safety/ArmedPolicy.h>
#include <QObject>
#include <QPointer>
#include <QStringList>
namespace kgc::app::net { class Http; }
namespace kgc::vehicle::command { struct CommandResult; }
namespace kgc::app::scheduler { class Timeout; }
namespace kgc::params {
class ParamDownload;
class ParamRebootReconnect;
class ParamMetadataLoader;
class ParamWrite;
/** 파일과 FC에서 값이 다른 파라미터 한 줄이에요.
 * 입력: 이름, FC 값 current, 파일 값 fileValue예요.
 * 출력: FileDiff::changed의 한 줄이에요.
 * 실패: 없어요.
 */
struct FileDiffRow { QString name; double current; double fileValue; };
/** 파일과 FC를 비교한 결과예요.
 * 입력: ParamService::compareFile이 채워요.
 * 출력: 바뀌는 값 목록과 같은 값·FC에 없음·읽기 전용·건너뜀·나쁜 줄·다른 컴포넌트 줄의 수예요.
 * 실패: 없어요.
 */
struct FileDiff {
    QList<FileDiffRow> changed;
    int same = 0, missing = 0, readOnly = 0, skipped = 0, badLines = 0, otherComponent = 0;
};
/** 기체 하나의 파라미터를 받고, 고치고, 쓰고, 파일로 옮겨요.
 * 입력: 부모인 Vehicle의 메시지·상태·송신 문·명령 큐, 시동 판정, HTTP예요.
 * 출력: Params 묶음과 내려받기·메타데이터 상태, 진행 중 Job이에요.
 * 실패: 작업 실패는 Job과 시그널로, 즉시 끝나는 함수는 Result로 알려요.
 * 흐름(내려받기):
 * 1. ParamDownload::start — 목록을 요청해요.
 * 2. ParamService가 받은 PARAM_VALUE를 Params에 넣어요.
 * 3. ParamDownload — 빠진 번호를 다시 요청하고 끝을 판정해요.
 */
class ParamService : public QObject {
    Q_OBJECT
public:
/** 메시지를 잇고 자동 내려받기와 메타데이터 준비를 시작해요.
 * 입력: 부모가 될 vehicle, 수명 동안 유지되는 policy·http·reconnect와 parent예요.
 * 출력: 시동 중이 아니면 내려받기를 시작한 서비스예요.
 * 실패: 없어요.
 */
    ParamService(kgc::vehicle::registry::Vehicle& vehicle, kgc::vehicle::safety::ArmedPolicy& policy,
                 kgc::app::net::Http& http, ParamRebootReconnect& reconnect, QObject* parent);
/** 진행 중인 작업을 끝내고 사라져요.
 * 입력: 없어요.
 * 출력: 살아 있는 Job을 Timeout으로 끝내 finished를 한 번 내요. 다시 받기 예약은 지워요.
 * 실패: 없어요. 부모 Vehicle은 이미 정리 중이라 건드리지 않아요.
 */
    ~ParamService() override;
/** 기체의 서비스를 찾아요.
 * 입력: 기체 vehicle이에요.
 * 출력: 자식 중의 ParamService이며, 소유권은 넘기지 않아요.
 * 실패: 없으면 nullptr예요.
 */
    static ParamService* of(kgc::vehicle::registry::Vehicle* vehicle);
/** 값 묶음을 읽어요.
 * 입력: 없어요.
 * 출력: 서비스가 소유하는 Params 포인터예요.
 * 실패: 없어요.
 */
    Params* params() const;
/** 내려받기 상태를 읽어요.
 * 입력: 없어요.
 * 출력: 지금 DownloadState예요.
 * 실패: 없어요.
 */
    DownloadState downloadState() const;
/** 메타데이터 상태를 읽어요.
 * 입력: 없어요.
 * 출력: 지금 MetadataState예요.
 * 실패: 없어요.
 */
    MetadataState metadataState() const;
/** 진행 중인 작업을 읽어요.
 * 입력: 없어요.
 * 출력: 진행 중 Job이며, 없으면 nullptr예요.
 * 실패: 없어요.
 */
    kgc::app::job::Job* job() const;
/** 재부팅이 필요한 값을 썼는지 읽어요.
 * 입력: 없어요.
 * 출력: 재부팅해야 적용되는 값을 쓴 뒤 재부팅 전이면 참이에요.
 * 실패: 없어요.
 */
    bool rebootNeeded() const;
/** 이번 회차에 받은 번호 수를 읽어요.
 * 입력: 없어요.
 * 출력: 받은 수예요.
 * 실패: 없어요.
 */
    int receivedCount() const;
/** FC가 알린 전체 수를 읽어요.
 * 입력: 없어요.
 * 출력: 마지막 param_count이며, 받기 전은 0이에요.
 * 실패: 없어요.
 */
    int totalCount() const;
/** 파라미터 전체를 다시 받아요.
 * 입력: 없어요.
 * 출력: 시동 중이어도 내려받기 Job을 시작해요.
 * 실패: Job이 이미 있으면 아무 일도 하지 않아요.
 * 흐름:
 * 1. Job을 만들고 상태를 Downloading으로 바꿔요.
 * 2. ParamDownload::start — PARAM_REQUEST_LIST를 보내요.
 */
    void refresh();
/** 대기값을 정해요.
 * 입력: 파라미터 이름 name과 새 값 value예요.
 * 출력: value가 FC 값과 같으면 대기값을 지워요. 범위 검사는 화면이 해요.
 * 실패: 이름이 없으면 NotFound, 읽기 전용이면 ReadOnly예요.
 */
    kgc::app::errors::Result<void> setPending(const QString& name, double value);
/** 대기값을 지워요.
 * 입력: 파라미터 이름 name이에요.
 * 출력: 대기값이 사라져요.
 * 실패: 없는 이름은 무시해요.
 */
    void clearPending(const QString& name);
/** 지금 대기값을 써도 되는지 판정해요.
 * 입력: 없어요.
 * 출력: 대기값이 2개 이상이면 ParamBulk, 아니면 ParamSingle로 ArmedPolicy가 낸 값이에요.
 * 실패: 없어요.
 */
    kgc::vehicle::safety::Decision writeDecision() const;
/** 대기값을 FC에 써요.
 * 입력: 없어요.
 * 출력: 끝나면 writeFinished를 내요.
 * 실패: 백업 실패와 시동 중단은 Job 실패로, 줄 실패는 writeFinished의 failed로 알려요.
 * 흐름:
 * 1. 전체 값을 paramsBackups에 백업해요.
 * 2. ParamWrite::start — 이름순으로 PARAM_SET을 보내고 돌아온 값을 확인해요.
 */
    void writePending();
/** FC 값을 파일로 저장해요.
 * 입력: 저장할 경로 path예요.
 * 출력: 성공 결과예요.
 * 실패: 쓰지 못하면 FileWrite예요.
 */
    kgc::app::errors::Result<void> saveFile(const QString& path) const;
/** 파일을 FC 값과 비교해요.
 * 입력: 읽을 경로 path예요.
 * 출력: 비교 결과예요.
 * 실패: 못 열면 FileOpen, 읽은 값이 없으면 FileEmpty예요.
 */
    kgc::app::errors::Result<FileDiff> compareFile(const QString& path) const;
/** 고른 파일 값을 대기값으로 올려요.
 * 입력: 고른 이름 names와 비교 결과 diff예요.
 * 출력: 고른 이름의 대기값이 정해져요. 쓰기는 사용자가 따로 눌러요.
 * 실패: 대기값으로 둘 수 없는 이름은 건너뛰어요.
 */
    void applyFile(const QStringList& names, const FileDiff& diff);
/** FC를 재부팅해요.
 * 입력: 없어요.
 * 출력: rebootFinished를 내요.
 * 실패: Job이 있으면 Busy, 시동 중이면 Armed, 거부 ACK나 링크 없음이면 RebootRefused예요.
 */
    void reboot();
signals:
/** 내려받기 상태가 바뀌었음을 알려요. 입력: 없어요. 출력: 다시 읽도록 알려요. 실패: 없어요. */
    void downloadStateChanged();
/** 메타데이터 상태가 바뀌었음을 알려요. 입력: 없어요. 출력: 다시 읽도록 알려요. 실패: 없어요. */
    void metadataStateChanged();
/** 진행 중 Job이 바뀌었음을 알려요. 입력: 없어요. 출력: 다시 읽도록 알려요. 실패: 없어요. */
    void jobChanged();
/** 재부팅 필요 여부가 바뀌었음을 알려요. 입력: 없어요. 출력: 다시 읽도록 알려요. 실패: 없어요. */
    void rebootNeededChanged();
/** 쓰기가 끝났음을 알려요.
 * 입력: 쓴 수 written과 쓰지 못한 이름 failed예요.
 * 출력: 시동 중단과 취소 때는 그때까지의 수예요. 백업 실패 때는 나오지 않아요.
 * 실패: 없어요.
 */
    void writeFinished(int written, QStringList failed);
/** 재부팅 요청이 끝났음을 알려요.
 * 입력: 성공이면 값 없는 결과, 아니면 오류 코드인 result예요.
 * 출력: 요청마다 많아야 한 번 나와요.
 * 실패: 없어요.
 */
    void rebootFinished(kgc::app::errors::Result<void> result);
private:
    void receive(const __mavlink_message& message);
    void autoRefresh();
    kgc::app::job::Job* beginJob(kgc::app::job::OperationKind kind);
    void finishDownload(DownloadState state);
    void regroup();
    void startMetadata();
    void linkUpdated();
    void rebootAnswered(const kgc::vehicle::command::CommandResult& result);
    void setDownloadState(DownloadState state);
    void setMetadataState(MetadataState state);
    void setRebootNeeded(bool value);
    QByteArray fileBytes() const;
    kgc::vehicle::registry::Vehicle& vehicle_;
    kgc::vehicle::safety::ArmedPolicy& policy_;
    Params* params_;
    ParamDownload* download_;
    ParamWrite* write_;
    ParamMetadataLoader* metadata_;
    ParamRebootReconnect& reconnect_;
    kgc::app::scheduler::Timeout* regroupTimer_;
    QPointer<kgc::app::job::Job> job_;
    DownloadState downloadState_ = DownloadState::Idle;
    MetadataState metadataState_ = MetadataState::None;
    bool rebootNeeded_ = false;
    bool rebootSent_ = false;
    bool refreshDeferred_ = false;
    bool regroupPending_ = false;
    bool metadataStarted_ = false;
    bool linkLost_ = false;
};
}
