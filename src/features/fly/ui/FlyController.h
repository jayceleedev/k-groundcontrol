// [0.6.0 5.11.8] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/fly/FlyService.h>
#include <kgc/ui/Slots.h>
#include <kgc/vehicle/registry/VehicleRegistry.h>
#include <QGeoCoordinate>
#include <QList>
#include <QMetaObject>
#include <QPointer>
#include <QQmlEngine>
#include <QVariantList>
namespace kgc::fly {
/** 활성 기체의 비행 상태를 상단바·여기로 가기 화면의 표시값으로 옮기고 FlyService를 불러요.
 * 입력: 기체 등록부(activeChanged마다 FlyService::of(active)와 그 VehicleStates에 다시 묶어요), 상단바 슬롯(숨김)이에요.
 * 출력: 기체·모드·시동·바쁨 글, 누를 수 있는 단추, 이륙 높이, 유도 한계, 목표·선회 표시값, 실패 토스트 글이에요.
 *       판정은 서비스가 맡아요. 화면 글은 읽을 때마다 그때 언어로 만들어요(번역 문맥 "Fly").
 * 실패: 실패는 toast로 알리고, 누르기 시작한 기체가 지금 활성 기체가 아니면 아무것도 보내지 않아요.
 */
class FlyController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool hasVehicle READ hasVehicle NOTIFY vehicleChanged)
    Q_PROPERTY(int sysid READ sysid NOTIFY vehicleChanged)
    Q_PROPERTY(QString vehicleText READ vehicleText NOTIFY vehicleChanged)
    Q_PROPERTY(QString modeText READ modeText NOTIFY stateChanged)
    Q_PROPERTY(double currentMode READ currentMode NOTIFY stateChanged)
    Q_PROPERTY(QVariantList modes READ modes NOTIFY stateChanged)
    Q_PROPERTY(bool armed READ armed NOTIFY stateChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY stateChanged)
    Q_PROPERTY(int takeoffKind READ takeoffKind NOTIFY stateChanged)
    Q_PROPERTY(QString takeoffFixedText READ takeoffFixedText NOTIFY stateChanged)
    Q_PROPERTY(QString goToHint READ goToHint NOTIFY stateChanged)
    Q_PROPERTY(bool canArm READ canArm NOTIFY readinessChanged)
    Q_PROPERTY(bool canDisarm READ canDisarm NOTIFY readinessChanged)
    Q_PROPERTY(bool canEmergencyStop READ canEmergencyStop NOTIFY readinessChanged)
    Q_PROPERTY(bool canTakeoff READ canTakeoff NOTIFY readinessChanged)
    Q_PROPERTY(bool canLand READ canLand NOTIFY readinessChanged)
    Q_PROPERTY(bool canRtl READ canRtl NOTIFY readinessChanged)
    Q_PROPERTY(bool canPause READ canPause NOTIFY readinessChanged)
    Q_PROPERTY(bool canSetMode READ canSetMode NOTIFY readinessChanged)
    Q_PROPERTY(bool canGoTo READ canGoTo NOTIFY readinessChanged)
    Q_PROPERTY(double takeoffAltitude READ takeoffAltitude WRITE setTakeoffAltitude NOTIFY takeoffAltitudeChanged)
    Q_PROPERTY(double guidedMaxDistanceM READ guidedMaxDistanceM WRITE setGuidedMaxDistanceM NOTIFY limitsChanged)
    Q_PROPERTY(double guidedMinAltitudeM READ guidedMinAltitudeM WRITE setGuidedMinAltitudeM NOTIFY limitsChanged)
    Q_PROPERTY(double guidedMaxAltitudeM READ guidedMaxAltitudeM WRITE setGuidedMaxAltitudeM NOTIFY limitsChanged)
    Q_PROPERTY(double guidedMaxDistanceMMin READ guidedMaxDistanceMMin CONSTANT)
    Q_PROPERTY(double guidedMaxDistanceMMax READ guidedMaxDistanceMMax CONSTANT)
    Q_PROPERTY(double guidedMinAltitudeMMin READ guidedMinAltitudeMMin CONSTANT)
    Q_PROPERTY(double guidedMinAltitudeMMax READ guidedMinAltitudeMMax CONSTANT)
    Q_PROPERTY(double guidedMaxAltitudeMMin READ guidedMaxAltitudeMMin CONSTANT)
    Q_PROPERTY(double guidedMaxAltitudeMMax READ guidedMaxAltitudeMMax CONSTANT)
    Q_PROPERTY(double guidedAltitudeFrom READ guidedAltitudeFrom NOTIFY limitsChanged)
    Q_PROPERTY(double guidedAltitudeTo READ guidedAltitudeTo NOTIFY limitsChanged)
    Q_PROPERTY(QGeoCoordinate vehicleCoordinate READ vehicleCoordinate NOTIFY vehicleCoordinateChanged)
    Q_PROPERTY(bool targetVisible READ targetVisible NOTIFY targetChanged)
    Q_PROPERTY(bool targetPending READ targetPending NOTIFY targetChanged)
    Q_PROPERTY(QGeoCoordinate targetCoordinate READ targetCoordinate NOTIFY targetChanged)
    Q_PROPERTY(QString targetText READ targetText NOTIFY targetChanged)
    Q_PROPERTY(bool loiterVisible READ loiterVisible NOTIFY loiterChanged)
    Q_PROPERTY(double loiterRadius READ loiterRadius NOTIFY loiterChanged)
    Q_PROPERTY(QString loiterText READ loiterText NOTIFY loiterChanged)
public:
    /** 입력: 기체 등록부와 상단바 슬롯이에요. 출력: 등록부가 소유하고 활성 기체에 묶인 컨트롤러예요. 실패: 중복 생성은 프로그래밍 오류예요. */
    FlyController(kgc::vehicle::registry::VehicleRegistry& vehicles, kgc::ui::TopbarSlot& topbar);
    /** 입력: QML 엔진과 스크립트 엔진이에요. 출력: C++가 소유하는 싱글턴이에요. 실패: 등록 전 호출은 프로그래밍 오류예요. */
    static FlyController* create(QQmlEngine* engine, QJSEngine* scriptEngine);
    /** 입력: 없어요. 출력: 활성 기체의 FlyService가 있으면 참이에요. 실패: 없어요. */
    bool hasVehicle() const;
    /** 입력: 없어요. 출력: 활성 기체 sysid이며 없으면 -1이에요. 실패: 없어요. */
    int sysid() const;
    /** 입력: 없어요. 출력: "기체 1" 또는 "기체 없음"이에요. 실패: 없어요. */
    QString vehicleText() const;
    /** 입력: 없어요. 출력: "모드: LOITER"이며, 기체가 없거나 종류를 모르거나 이름이 비면 "모드: —"예요. 실패: 없어요. */
    QString modeText() const;
    /** 입력: 없어요. 출력: 지금 모드 번호이며 모르면 -1이에요. 실패: 없어요. */
    double currentMode() const;
    /** 입력: 없어요. 출력: 모드 창 목록이며 항목은 customMode·name·choosable(공중의 스틱 모드는 거짓)이에요. 실패: 없어요. */
    QVariantList modes() const;
    /** 입력: 없어요. 출력: 활성 기체가 시동 중이면 참이에요. 실패: 없어요. */
    bool armed() const;
    /** 입력: 없어요. 출력: 비행 명령이 응답·확인을 기다리면 참이에요. 실패: 없어요. */
    bool busy() const;
    /** 입력: 없어요. 출력: 이륙 방식 0 None·1 WithAltitude·2 FixedAltitude예요. 실패: 없어요. */
    int takeoffKind() const;
    /** 입력: 없어요. 출력: 고정 고도 이륙 안내 "기체 설정 TKOFF_ALT(50 m)까지 올라가요"예요. 실패: 값을 모르면 고도 없이 써요. */
    QString takeoffFixedText() const;
    /** 입력: 없어요. 출력: 도착 뒤 도는지·떠 있는지 안내 글이에요. 실패: 없어요. */
    QString goToHint() const;
    /** 상단바 단추와 창 확인 단추를 누를 수 있는지 서비스의 readiness에서 읽어요(아홉 동작 각각).
     * 입력: 없어요. 출력: 그 동작을 지금 누를 수 있으면 참이에요. 실패: 기체가 없으면 모두 거짓이에요.
     */
    bool canArm() const;
    bool canDisarm() const;
    bool canEmergencyStop() const;
    bool canTakeoff() const;
    bool canLand() const;
    bool canRtl() const;
    bool canPause() const;
    bool canSetMode() const;
    bool canGoTo() const;
    /** 입력: 없어요. 출력: 이번 실행에서 마지막으로 보낸 이륙 높이를 지금 고도 한계 안으로 자른 값이에요. 실패: 없어요. */
    double takeoffAltitude() const;
    /** 입력: 높이 value예요. 출력: 이번 실행 동안 기억해요(설정으로 저장하지 않아요). 실패: 없어요. */
    void setTakeoffAltitude(double value);
    /** 유도 한계 설정 셋(최대 거리·최저 고도·최고 고도)을 생성 접근자로 읽어요.
     * 입력: 없어요. 출력: 저장된 설정 값이에요. 실패: 저장값이 범위 밖이면 설정 기본값이에요.
     */
    double guidedMaxDistanceM() const;
    double guidedMinAltitudeM() const;
    double guidedMaxAltitudeM() const;
    /** 유도 한계 설정 셋을 생성 접근자로 써요(앱 설정 칸이 불러요).
     * 입력: 새 값 value예요. 출력: 설정에 쓰고, 바뀌면 limitsChanged가 와요. 실패: 범위 밖 값은 저장되지 않아요.
     */
    void setGuidedMaxDistanceM(double value);
    void setGuidedMinAltitudeM(double value);
    void setGuidedMaxAltitudeM(double value);
    /** 유도 한계 설정 셋의 허용 범위(최솟값·최댓값 여섯)를 생성 접근자로 읽어요.
     * 입력: 없어요. 출력: 설정 정의의 min·max예요. 실패: 없어요.
     */
    double guidedMaxDistanceMMin() const;
    double guidedMaxDistanceMMax() const;
    double guidedMinAltitudeMMin() const;
    double guidedMinAltitudeMMax() const;
    double guidedMaxAltitudeMMin() const;
    double guidedMaxAltitudeMMax() const;
    /** 고도 칸이 받는 범위(홈 기준 최저·최고 고도)를 guidedLimits()에서 읽어요.
     * 입력: 없어요. 출력: 최저·최고 고도예요(설정의 최저가 최고보다 크면 바꾼 순서). 실패: 없어요.
     */
    double guidedAltitudeFrom() const;
    double guidedAltitudeTo() const;
    /** 입력: 없어요. 출력: 활성 기체 위치이며 모르면 무효 좌표예요. 실패: 없어요. */
    QGeoCoordinate vehicleCoordinate() const;
    /** 지도에 그릴 여기로 가기 목표 표식의 값(보이기·대기·좌표·고도 글)을 서비스의 target에서 읽어요.
     * 입력: 없어요. 출력: 목표가 Sending·Accepted·Confirmed면 보이기 참, Sending이면 대기(점선) 참, 좌표와 "목표 50 m" 글이에요.
     * 실패: 기체나 목표가 없으면 거짓·무효 좌표·빈 글이에요.
     */
    bool targetVisible() const;
    bool targetPending() const;
    QGeoCoordinate targetCoordinate() const;
    QString targetText() const;
    /** 지도에 그릴 선회 원의 값(보이기·반지름·글)을 서비스의 loiter에서 읽어요.
     * 입력: 없어요. 출력: 선회 원을 그릴지, 반지름(m), "반지름 80 m" 글이에요.
     * 실패: 기체가 없거나 원이 없으면 거짓·0·빈 글이에요.
     */
    bool loiterVisible() const;
    double loiterRadius() const;
    QString loiterText() const;
    /** 시동을 걸어요.
     * 입력: 누르기 시작한 때의 기체 sysid예요. 출력: 보냈으면 참이에요. 실패: 활성 기체가 아니면 보내지 않고 거짓이에요.
     * 흐름:
     * 1. sysid가 활성 기체인지 봐요.
     * 2. FlyService::arm — 시동 단계를 보내고 결과는 actionFinished로 와요.
     */
    Q_INVOKABLE bool arm(int sysid);
    /** 시동을 꺼요.
     * 입력: 누르기 시작한 때의 기체 sysid예요. 출력: 보냈으면 참이에요. 실패: 활성 기체가 아니면 보내지 않고 거짓이에요.
     * 흐름:
     * 1. sysid가 활성 기체인지 봐요.
     * 2. FlyService::disarm — 진행 중 명령에 끼어들어 시동 끄기를 보내요.
     */
    Q_INVOKABLE bool disarm(int sysid);
    /** 공중에서 모터를 바로 꺼요.
     * 입력: 누르기 시작한 때의 기체 sysid예요. 출력: 보냈으면 참이에요. 실패: 활성 기체가 아니면 보내지 않고 거짓이에요.
     * 흐름:
     * 1. sysid가 활성 기체인지 봐요.
     * 2. FlyService::emergencyStop — 진행 중 명령에 끼어들어 COMPONENT_ARM_DISARM(param2 21196)을 보내요.
     */
    Q_INVOKABLE bool emergencyStop(int sysid);
    /** 착륙을 시작해요.
     * 입력: 누르기 시작한 때의 기체 sysid예요. 출력: 보냈으면 참이에요. 실패: 활성 기체가 아니면 보내지 않고 거짓이에요.
     * 흐름:
     * 1. sysid가 활성 기체인지 봐요.
     * 2. FlyService::land — 플러그인의 착륙 단계를 보내고 모드 확인을 기다려요.
     */
    Q_INVOKABLE bool land(int sysid);
    /** RTL을 시작해요.
     * 입력: 누르기 시작한 때의 기체 sysid예요. 출력: 보냈으면 참이에요. 실패: 활성 기체가 아니면 보내지 않고 거짓이에요.
     * 흐름:
     * 1. sysid가 활성 기체인지 봐요.
     * 2. FlyService::rtl — 플러그인의 RTL 단계를 보내고 모드 확인을 기다려요.
     */
    Q_INVOKABLE bool rtl(int sysid);
    /** 그 자리에 멈춰요(일시정지).
     * 입력: 누르기 시작한 때의 기체 sysid예요. 출력: 보냈으면 참이에요. 실패: 활성 기체가 아니면 보내지 않고 거짓이에요.
     * 흐름:
     * 1. sysid가 활성 기체인지 봐요.
     * 2. FlyService::pause — 플러그인의 일시정지 단계(기체 모양·펌웨어 버전에 따라)를 보내고 모드 확인을 기다려요.
     */
    Q_INVOKABLE bool pause(int sysid);
    /** 이륙해요.
     * 입력: 누르기 시작한 때의 기체 sysid, 지금 자리 위 높이 height(FixedAltitude면 NaN)예요.
     * 출력: 보냈으면 참이에요. WithAltitude면 보낸 높이를 takeoffAltitude로 기억해요(성공 여부와 상관없이).
     * 실패: 활성 기체가 아니면 보내지 않고 거짓이에요.
     * 흐름:
     * 1. sysid가 활성 기체인지 봐요.
     * 2. takeoffAltitude = height예요(WithAltitude만).
     * 3. FlyService::takeoff — 높이 검사와 고도 틀 변환 뒤 모드·시동·이륙 단계를 보내요.
     */
    Q_INVOKABLE bool takeoff(int sysid, double height);
    /** 모드를 바꿔요.
     * 입력: 누르기 시작한 때의 기체 sysid, 모드 번호 customMode예요. 출력: 보냈으면 참이에요.
     * 실패: 활성 기체가 아니면 보내지 않고 거짓이에요.
     * 흐름:
     * 1. sysid가 활성 기체인지 봐요.
     * 2. FlyService::setMode — 고를 수 있는 모드인지 보고 DO_SET_MODE를 보내요.
     */
    Q_INVOKABLE bool setMode(int sysid, double customMode);
    /** 고른 곳으로 기체를 보내요.
     * 입력: 창을 연 때의 기체 sysid, 위도 lat·경도 lon, 홈 기준 고도 altitude예요. 출력: 보냈으면 참이에요.
     * 실패: 활성 기체가 아니면 거짓(토스트 없음), 검사 실패면 토스트를 내고 거짓이에요.
     * 흐름:
     * 1. sysid가 활성 기체인지 봐요.
     * 2. FlyService::goTo — 검사, 플러그인 단계, GoToTracker::begin, FlyRunner로 보내요.
     */
    Q_INVOKABLE bool goTo(int sysid, double lat, double lon, double altitude);
    /** 입력: 위도·경도·고도예요. 출력: 여기로 가기 창에 보일 문제 글이며 괜찮거나 NotReady면 빈 값이에요. 실패: 없어요. */
    Q_INVOKABLE QString goToProblem(double lat, double lon, double altitude) const;
    /** 입력: 위도·경도예요. 출력: 기체에서의 거리 글("259 m")이에요. 실패: 기체 위치를 모르면 "—"예요. */
    Q_INVOKABLE QString distanceText(double lat, double lon) const;
    /** 입력: 위도·경도예요. 출력: "35.160500, 129.162200"이에요. 실패: 없어요. */
    Q_INVOKABLE QString positionText(double lat, double lon) const;
    /** 입력: 없어요. 출력: 활성 기체 홈 기준 고도를 0.1 m로 반올림해 고도 한계 안으로 자른 값이며, 모르면 최저 고도예요. 실패: 없어요. */
    Q_INVOKABLE double defaultGoToAltitude() const;
signals:
    /** 활성 기체가 바뀌었음을 알려요(그때만 한 번). */
    void vehicleChanged();
    /** 모드·시동·바쁨·이륙 방식·안내 글의 모음이 바뀌었음을 알려요. */
    void stateChanged();
    /** 누를 수 있는 단추가 바뀌었음을 알려요. */
    void readinessChanged();
    /** 이륙 높이가 바뀌었음을 알려요. */
    void takeoffAltitudeChanged();
    /** 유도 한계 설정이 바뀌었음을 알려요. */
    void limitsChanged();
    /** 활성 기체 위치가 바뀌었음을 알려요. */
    void vehicleCoordinateChanged();
    /** 여기로 가기 목표가 바뀌었음을 알려요. */
    void targetChanged();
    /** 선회 원이 바뀌었음을 알려요. */
    void loiterChanged();
    /** 활성 기체 동작의 실패 글을 알려요. 입력: "<동작 글>. <오류 글>"(+ 줄 바꿈 "기체: …")이에요. */
    void toast(const QString& text);
private:
    void rebind();
    void refreshState();
    void refreshCoordinate();
    void applyHidden();
    void showFailure(FlyActionKind kind, const kgc::app::errors::Result<void>& result, const QString& reason);
    bool isActive(int sysid) const;
    QVariantList stateSnapshot() const;
    kgc::vehicle::registry::VehicleRegistry& vehicles_;
    kgc::ui::TopbarSlot& topbar_;
    QPointer<FlyService> service_;
    QList<QMetaObject::Connection> connections_;
    QVariantList state_;
    QGeoCoordinate coordinate_;
    double takeoffAltitude_;
    inline static QPointer<FlyController> instance_;
};
}
