// [0.4.0 5.4 6.1–6.7] specs/features/0.4.0-sitl.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <kgc/sitl/SitlTypes.h>
#include <QDateTime>
#include <QObject>
#include <QStringList>
#include <memory>
#include <optional>
namespace kgc::app::job { class Job; }
namespace kgc::app::net { class Http; }
namespace kgc::comm::links { class LinkManager; }
namespace kgc::sitl {
/** ArduPilot SITL을 내려받고 이 PC에서 띄워 UDP 링크로 이어요.
 * 입력: 전역 LinkManager·Http와 화면의 요청이에요.
 * 출력: 상태·내려받기 Job·출력 줄·링크 여부를 읽기 함수와 시그널로 내놓아요.
 * 실패: 받아들이지 않은 요청은 Result 오류로, 진행 중 실패는 Failed와 lastError로 알려요.
 */
class SitlService : public QObject {
    Q_OBJECT
public:
/** 멈춘 서비스를 만들어요.
 * 입력: 링크 관리자 links, HTTPS 통로 http, 그 둘보다 먼저 지워지는 수명 객체 parent예요.
 * 출력: Idle 상태의 서비스예요. 앱이 끝날 때(aboutToQuit) stop을 불러요.
 * 실패: 없어요.
 */
    SitlService(kgc::comm::links::LinkManager& links, kgc::app::net::Http& http, QObject* parent);
/** 남은 시뮬레이터 프로세스만 멈추고 지워져요.
 * 입력: 없어요.
 * 출력: LinkManager는 부르지 않아요. 프로세스가 남지 않아요.
 * 실패: 없어요.
 */
    ~SitlService() override;
    /** 입력: 없어요. 출력: 지금 상태예요. 실패: 없어요. */
    SitlState state() const;
    /** 입력: 없어요. 출력: 내려받는 중의 Job이며, 없으면 nullptr예요. 소유권은 넘기지 않아요. 실패: 없어요. */
    kgc::app::job::Job* job() const;
/** 실행 파일과 defaults.parm이 캐시에 다 있는지 봐요.
 * 입력: 기체 vehicle과 배포판 release예요.
 * 출력: 둘 다 있으면 참이에요.
 * 실패: 없어요.
 */
    bool cached(SitlVehicle vehicle, SitlRelease release) const;
/** 캐시에 받아 둔 실행 파일의 수정 시각을 읽어요.
 * 입력: 기체 vehicle과 배포판 release예요.
 * 출력: 실행 파일의 수정 시각이며, 없으면 무효한 값이에요.
 * 실패: 없어요.
 */
    QDateTime cachedAt(SitlVehicle vehicle, SitlRelease release) const;
    /** 입력: 없어요. 출력: 이 서비스가 연 UDP 링크가 지금 열려 있으면 참이에요. 실패: 없어요. */
    bool linkOpenedBySitl() const;
    /** 입력: 없어요. 출력: LinkManager::linkId() >= 0이고 linkOpenedBySitl이 거짓이면 참이에요. 실패: 없어요. */
    bool linkBusy() const;
    /** 입력: 없어요. 출력: 마지막 요청하지 않은 종료의 코드이며, 강제 종료(CrashExit)나 없으면 -1이에요. 실패: 없어요. */
    int lastExitCode() const;
    /** 입력: 없어요. 출력: 시뮬레이터 출력의 마지막 줄들(많아야 200줄)이에요. 실패: 없어요. */
    QStringList output() const;
    /** 입력: 없어요. 출력: Failed의 까닭이며, 없으면 빈 값이에요. 실패: 없어요. */
    std::optional<kgc::app::errors::ErrorCode> lastError() const;
/** 시뮬레이터를 시작해요. 캐시가 없으면 먼저 내려받아요.
 * 입력: 기체 vehicle, 배포판 release, 홈 home, 속도 배수 speedup, 값 초기화 wipe예요.
 * 출력: 받아들이면 설정 일곱 개를 저장하고 Downloading 또는 Starting을 거쳐 Running으로 가요.
 * 실패: 진행 중이면 Busy, 홈·속도가 범위 밖이면 BadHome, 다른 링크가 있으면 LinkBusy를 바로 돌려줘요(상태 그대로).
 *       그 뒤 실패는 Failed와 lastError(DownloadFailed·StartFailed·LinkBusy·LinkFailed·Crashed)예요.
 * 흐름:
 * 1. checkHome·settings — 받아들일 수 있는지 보고 일곱 값을 저장해요.
 * 2. SitlDownload — 캐시가 없으면 Http::download로 sitlFiles를 차례로 받아요.
 * 3. Paths::sitl — 실행 폴더를 만들고 defaults.parm을 덮어 복사해요.
 * 4. Simulator::start — sitlArguments로 실행 파일을 띄워요.
 * 5. LinkManager::connectLink — started 뒤 UDP를 열고 linkOpened면 Running이에요.
 */
    kgc::app::errors::Result<void> start(SitlVehicle vehicle, SitlRelease release, const SitlHome& home,
                                         int speedup, bool wipe);
/** 내려받기나 실행을 멈춰요.
 * 입력: 없어요.
 * 출력: Downloading이면 Job을 취소하고, Starting·Running이면 이 서비스의 링크를 끊고 프로세스를 멈춘 뒤 Idle이에요.
 * 실패: 다른 상태면 아무 일도 하지 않아요.
 * 흐름:
 * 1. Job::cancel — 내려받는 중이면 취소를 요청해요.
 * 2. LinkManager::disconnectLink — 이 서비스가 연 링크가 열려 있거나 열리는 중일 때만 끊어요.
 * 3. Simulator::stop — 프로세스를 멈추고 3초까지 기다려요.
 */
    void stop();
/** 캐시가 있어도 파일을 다시 받아요.
 * 입력: 기체 vehicle과 배포판 release예요.
 * 출력: 끝나면 Idle 또는 Failed(DownloadFailed)이고 cacheChanged를 내요. 취소면 Idle이에요.
 * 실패: Downloading·Starting·Running이면 Busy예요.
 * 흐름:
 * 1. SitlDownload — Http::download로 .part 폴더에 받은 뒤 캐시 폴더와 바꿔요.
 */
    kgc::app::errors::Result<void> redownload(SitlVehicle vehicle, SitlRelease release);
/** 실행 중인데 링크가 닫혔을 때 UDP를 다시 열어요.
 * 입력: 없어요.
 * 출력: 접수하면 linkOpened 뒤 linkOpenedBySitl이 참이 돼요.
 * 실패: Running이 아니면 Busy, 다른 링크가 있으면 LinkBusy예요.
 * 흐름:
 * 1. LinkManager::connectLink — UDP 링크를 다시 열어요.
 */
    kgc::app::errors::Result<void> reconnect();
signals:
    /** 입력: 없어요. 출력: state와 lastError를 다시 읽도록 알려요. 실패: 없어요. */
    void stateChanged();
    /** 입력: 없어요. 출력: job을 다시 읽도록 알려요. 실패: 없어요. */
    void jobChanged();
    /** 입력: 없어요. 출력: output을 다시 읽도록 알려요. 많아야 200ms에 한 번이에요. 실패: 없어요. */
    void outputChanged();
    /** 입력: 없어요. 출력: linkOpenedBySitl을 다시 읽도록 알려요. 실패: 없어요. */
    void linkOpenedBySitlChanged();
    /** 입력: 없어요. 출력: linkBusy를 다시 읽도록 알려요. 실패: 없어요. */
    void linkBusyChanged();
    /** 입력: 없어요. 출력: cached·cachedAt을 다시 읽도록 알려요. 실패: 없어요. */
    void cacheChanged();
private:
    struct Private;
    std::unique_ptr<Private> d_;
    void beginDownload(SitlVehicle vehicle, SitlRelease release, bool launchAfter);
    void downloadFinished(const kgc::app::errors::Result<void>& result, bool launchAfter);
    void launch();
    void connectSitlLink();
    void wireLinks();
    void wireSimulator();
    void onLinkOpened(int linkId);
    void onLinkFailed(kgc::app::errors::ErrorCode error);
    void onLinkClosed(int linkId);
    void onExited(int exitCode, bool crashed, bool requested);
    void onLine(QString text);
    void setState(SitlState state, std::optional<kgc::app::errors::ErrorCode> error = std::nullopt);
    void refreshLinkFlags();
};
}
