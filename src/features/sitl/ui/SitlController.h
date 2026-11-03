// [0.4.0 5.4 6.8] specs/features/0.4.0-sitl.md
#pragma once
#include <kgc/sitl/SitlService.h>
#include <kgc/app/job/Job.h>
#include <QPointer>
#include <QQmlEngine>
#include <QVariantList>
namespace kgc::sitl {
/** 시뮬레이션 패널의 입력값을 들고 서비스 상태를 QML에 옮겨요.
 * 입력: 전역 서비스예요. 처음 입력값은 저장된 설정에서 읽어요.
 * 출력: 상태·입력·표시 글자 프로퍼티와 요청 함수예요. 판정은 서비스가 맡아요.
 * 실패: 없어요.
 */
class SitlController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(kgc::sitl::SitlState state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool editable READ editable NOTIFY stateChanged)
    Q_PROPERTY(kgc::app::job::Job* job READ job NOTIFY jobChanged)
    Q_PROPERTY(bool linkOpenedBySitl READ linkOpenedBySitl NOTIFY linkChanged)
    Q_PROPERTY(bool linkBusy READ linkBusy NOTIFY linkChanged)
    Q_PROPERTY(int vehicle READ vehicle NOTIFY inputsChanged)
    Q_PROPERTY(int release READ release NOTIFY inputsChanged)
    Q_PROPERTY(double homeLatitude READ homeLatitude WRITE setHomeLatitude NOTIFY inputsChanged)
    Q_PROPERTY(double homeLongitude READ homeLongitude WRITE setHomeLongitude NOTIFY inputsChanged)
    Q_PROPERTY(double homeAltitude READ homeAltitude WRITE setHomeAltitude NOTIFY inputsChanged)
    Q_PROPERTY(int homeHeading READ homeHeading WRITE setHomeHeading NOTIFY inputsChanged)
    Q_PROPERTY(int speedup READ speedup WRITE setSpeedup NOTIFY inputsChanged)
    Q_PROPERTY(int speedupMin READ speedupMin CONSTANT)
    Q_PROPERTY(int speedupMax READ speedupMax CONSTANT)
    Q_PROPERTY(bool wipe READ wipe WRITE setWipe NOTIFY inputsChanged)
    Q_PROPERTY(QVariantList badHomeFields READ badHomeFields NOTIFY inputsChanged)
    Q_PROPERTY(bool canStart READ canStart NOTIFY canStartChanged)
    Q_PROPERTY(QString cachedAt READ cachedAt NOTIFY cacheChanged)
    Q_PROPERTY(QString outputText READ outputText NOTIFY outputChanged)
public:
/** 서비스를 부모로 삼고 저장된 설정으로 입력값을 채워요.
 * 입력: 서비스예요.
 * 출력: 서비스가 소유하는 컨트롤러예요.
 * 실패: 중복 등록은 프로그래밍 오류예요.
 */
    explicit SitlController(SitlService& service);
/** 등록된 객체를 QML 엔진에 제공해요.
 * 입력: QML 엔진과 스크립트 엔진이에요.
 * 출력: C++가 소유하는 싱글턴 포인터예요.
 * 실패: 등록 전 호출이나 다른 스레드의 엔진은 프로그래밍 오류예요.
 */
    static SitlController* create(QQmlEngine* engine, QJSEngine* scriptEngine);
    /** 입력: 없어요. 출력: 서비스의 상태예요. 실패: 없어요. */
    SitlState state() const { return service_.state(); }
    /** 입력: 없어요. 출력: Idle·Failed라서 입력을 바꿀 수 있으면 참이에요. 실패: 없어요. */
    bool editable() const;
    /** 입력: 없어요. 출력: 서비스의 내려받기 Job이며, 없으면 nullptr예요. 실패: 없어요. */
    kgc::app::job::Job* job() const { return service_.job(); }
    /** 입력: 없어요. 출력: 서비스의 linkOpenedBySitl이에요. 실패: 없어요. */
    bool linkOpenedBySitl() const { return service_.linkOpenedBySitl(); }
    /** 입력: 없어요. 출력: 서비스의 linkBusy예요. 실패: 없어요. */
    bool linkBusy() const { return service_.linkBusy(); }
    /** 입력: 없어요. 출력: 고른 기체(SitlVehicle 값)예요. 실패: 없어요. */
    int vehicle() const { return int(vehicle_); }
    /** 입력: 없어요. 출력: 고른 배포판(SitlRelease 값)이에요. 실패: 없어요. */
    int release() const { return int(release_); }
    /** 입력: 없어요. 출력: 입력한 위도예요. 실패: 없어요. */
    double homeLatitude() const { return home_.latitude; }
    /** 입력: 없어요. 출력: 입력한 경도예요. 실패: 없어요. */
    double homeLongitude() const { return home_.longitude; }
    /** 입력: 없어요. 출력: 입력한 고도(m)예요. 실패: 없어요. */
    double homeAltitude() const { return home_.altitude; }
    /** 입력: 없어요. 출력: 입력한 기수(도)예요. 실패: 없어요. */
    int homeHeading() const { return home_.heading; }
    /** 입력: 없어요. 출력: 입력한 속도 배수예요. 실패: 없어요. */
    int speedup() const { return speedup_; }
    /** 입력: 없어요. 출력: 설정 정의의 속도 배수 하한이에요. 실패: 없어요. */
    int speedupMin() const;
    /** 입력: 없어요. 출력: 설정 정의의 속도 배수 상한이에요. 실패: 없어요. */
    int speedupMax() const;
    /** 입력: 없어요. 출력: 값 초기화를 골랐으면 참이에요. 늘 거짓으로 시작해요. 실패: 없어요. */
    bool wipe() const { return wipe_; }
    /** 입력: 위도 value예요. 출력: 입력값을 바꾸고 inputsChanged를 내요. 실패: 없어요. */
    void setHomeLatitude(double value);
    /** 입력: 경도 value예요. 출력: 입력값을 바꾸고 inputsChanged를 내요. 실패: 없어요. */
    void setHomeLongitude(double value);
    /** 입력: 고도 value예요. 출력: 입력값을 바꾸고 inputsChanged를 내요. 실패: 없어요. */
    void setHomeAltitude(double value);
    /** 입력: 기수 value예요. 출력: 입력값을 바꾸고 inputsChanged를 내요. 실패: 없어요. */
    void setHomeHeading(int value);
    /** 입력: 속도 배수 value예요. 출력: 입력값을 바꾸고 inputsChanged를 내요. 실패: 없어요. */
    void setSpeedup(int value);
    /** 입력: 값 초기화 여부 value예요. 출력: 입력값을 바꾸고 inputsChanged를 내요. 실패: 없어요. */
    void setWipe(bool value);
    /** 입력: 없어요. 출력: checkHome이 돌려준 칸(SitlHomeField 값)의 목록이에요. 실패: 없어요. */
    QVariantList badHomeFields() const;
    /** 입력: 없어요. 출력: 입력을 바꿀 수 있고 홈이 맞고 다른 연결이 없으면 참이에요. 실패: 없어요. */
    bool canStart() const;
    /** 입력: 없어요. 출력: 고른 기체·배포판의 캐시 시각("yyyy-MM-dd HH:mm")이며, 캐시가 없으면 빈 값이에요. 실패: 없어요. */
    QString cachedAt() const;
    /** 입력: 없어요. 출력: 시뮬레이터 출력 줄을 줄바꿈으로 이은 글자예요. 실패: 없어요. */
    QString outputText() const { return service_.output().join(QLatin1Char('\n')); }
    /** 입력: 없어요. 출력: 현재 언어의 홈 범위 오류 문구예요. 실패: 없어요. */
    Q_INVOKABLE QString badHomeText() const;
/** 실패 배너의 글자를 현재 언어로 만들어요.
 * 입력: 없어요.
 * 출력: errorText(lastError)이며, Crashed이고 종료 코드가 있으면 " (종료 코드 N)"을 붙여요. 실패가 없으면 빈 값이에요.
 * 실패: 없어요.
 */
    Q_INVOKABLE QString failureText() const;
/** 기체를 골라요.
 * 입력: SitlVehicle 값 value예요.
 * 출력: 입력값과 캐시 글자를 바꿔요.
 * 실패: 범위 밖이거나 바꿀 수 없는 상태면 무시해요.
 * 흐름:
 * 1. SitlService::cachedAt — 고른 기체의 캐시 시각을 다시 읽어요.
 */
    Q_INVOKABLE void chooseVehicle(int value);
/** 배포판을 골라요.
 * 입력: SitlRelease 값 value예요.
 * 출력: 입력값과 캐시 글자를 바꿔요.
 * 실패: 범위 밖이거나 바꿀 수 없는 상태면 무시해요.
 * 흐름:
 * 1. SitlService::cachedAt — 고른 배포판의 캐시 시각을 다시 읽어요.
 */
    Q_INVOKABLE void chooseRelease(int value);
/** 홈 네 칸을 설정 기본값(해운대)으로 채워요.
 * 입력: 없어요.
 * 출력: 생성된 <이름>Default() 네 개로 입력값을 바꿔요. 저장하지는 않아요.
 * 실패: 바꿀 수 없는 상태면 무시해요.
 * 흐름:
 * 1. settings::homeLatitudeDefault 등 — JSON의 기본값을 읽어요.
 */
    Q_INVOKABLE void resetHome();
/** 시작 단추를 눌렀을 때 시작하거나 정지해요.
 * 입력: 없어요.
 * 출력: Starting·Running이면 정지, 아니면 지금 입력값으로 시작을 요청해요.
 * 실패: 서비스가 받아들이지 않으면 상태를 바꾸지 않아요(화면이 먼저 막아요).
 * 흐름:
 * 1. SitlService::stop 또는 SitlService::start — 실행을 멈추거나 내려받기·실행·연결을 시작해요.
 */
    Q_INVOKABLE void startOrStop();
/** 파일을 다시 받아요.
 * 입력: 없어요.
 * 출력: 고른 기체·배포판을 다시 받아요.
 * 실패: 진행 중이면 서비스가 Busy로 거절해요.
 * 흐름:
 * 1. SitlService::redownload — 캐시가 있어도 다시 받아요.
 */
    Q_INVOKABLE void redownload();
/** 닫힌 UDP 링크를 다시 열어요.
 * 입력: 없어요.
 * 출력: 서비스가 링크를 다시 열어요.
 * 실패: 다른 연결이 있으면 서비스가 LinkBusy로 거절해요.
 * 흐름:
 * 1. SitlService::reconnect — UDP를 다시 열어요.
 */
    Q_INVOKABLE void reconnect();
signals:
    /** 입력: 없어요. 출력: state·editable을 다시 읽도록 알려요. 실패: 없어요. */
    void stateChanged();
    /** 입력: 없어요. 출력: job을 다시 읽도록 알려요. 실패: 없어요. */
    void jobChanged();
    /** 입력: 없어요. 출력: 링크 표시값을 다시 읽도록 알려요. 실패: 없어요. */
    void linkChanged();
    /** 입력: 없어요. 출력: 입력값을 다시 읽도록 알려요. 실패: 없어요. */
    void inputsChanged();
    /** 입력: 없어요. 출력: canStart를 다시 읽도록 알려요. 실패: 없어요. */
    void canStartChanged();
    /** 입력: 없어요. 출력: 캐시 글자를 다시 읽도록 알려요. 실패: 없어요. */
    void cacheChanged();
    /** 입력: 없어요. 출력: 출력 글자를 다시 읽도록 알려요. 실패: 없어요. */
    void outputChanged();
private:
    void changeInputs();
    SitlService& service_;
    SitlVehicle vehicle_;
    SitlRelease release_;
    SitlHome home_;
    int speedup_;
    bool wipe_ = false;
    inline static QPointer<SitlController> instance_;
};
}
