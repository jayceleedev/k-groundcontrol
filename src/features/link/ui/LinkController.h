// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
#pragma once
#include <kgc/link/LinkService.h>
#include <kgc/vehicle/registry/VehicleRegistry.h>
#include <QQmlEngine>
#include <QPointer>
#include <QVariantList>
namespace kgc::link {
/** 연결 선택과 활성 기체를 QML에 제공해요.
 * 입력: 전역 연결 서비스예요.
 * 출력: 원본 상태에서 읽은 화면 값이에요.
 * 실패: 없어요.
 */
class LinkController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(kgc::link::LinkService::ConnectionState connectionState READ connectionState NOTIFY connectionStateChanged)
    Q_PROPERTY(bool canConnect READ canConnect NOTIFY connectionStateChanged)
    Q_PROPERTY(QVariantList ports READ ports NOTIFY portsChanged)
    Q_PROPERTY(int portIndex READ portIndex NOTIFY portsChanged)
    Q_PROPERTY(bool udp READ udp NOTIFY portsChanged)
    Q_PROPERTY(QVariantList baudRates READ baudRates CONSTANT)
    Q_PROPERTY(int baudRate READ baudRate NOTIFY baudRateChanged)
    Q_PROPERTY(int sysid READ sysid NOTIFY vehicleChanged)
    Q_PROPERTY(QString firmwareName READ firmwareName NOTIFY vehicleChanged)
    Q_PROPERTY(bool lost READ lost NOTIFY vehicleChanged)
public:
/** 서비스를 부모로 삼아 화면 수명을 묶어요.
 * 입력: 서비스예요.
 * 출력: 서비스가 소유하는 컨트롤러예요.
 * 실패: 중복 등록은 프로그래밍 오류예요.
 */
    explicit LinkController(LinkService& service);
/** 등록된 객체를 QML 엔진에 제공해요.
 * 입력: QML 엔진과 스크립트 엔진이에요.
 * 출력: C++가 소유하는 싱글턴 포인터예요.
 * 실패: 등록 전 호출이나 다른 스레드의 엔진은 프로그래밍 오류예요.
 */
    static LinkController* create(QQmlEngine* engine, QJSEngine* scriptEngine);
/** 서비스의 연결 상태를 읽어요.
 * 입력: 없어요.
 * 출력: 서비스의 Idle·Opening·Open·Failed 상태예요.
 * 실패: 없어요.
 */
    LinkService::ConnectionState connectionState() const { return service_.connectionState(); }
/** 서비스가 판단한 연결 설정 가능 여부를 읽어요.
 * 입력: 없어요.
 * 출력: 선택과 연결 단추에 적용할 서비스의 판단이에요.
 * 실패: 없어요.
 */
    bool canConnect() const { return service_.canConnect(); }
/** 포트 이름과 설명을 원본 값으로 제공해요.
 * 입력: 없어요.
 * 출력: 첫 항목은 빈 포트 이름인 UDP이고, 뒤는 portName·description 값의 목록이에요.
 * 실패: 없어요.
 */
    QVariantList ports() const;
/** 포트 이름에 맞는 목록 위치를 찾아요.
 * 입력: 없어요.
 * 출력: 고른 이름의 현재 위치이며 UDP는 0이에요.
 * 실패: 없어요.
 */
    int portIndex() const;
/** UDP 선택 여부를 읽어요.
 * 입력: 없어요.
 * 출력: UDP를 골랐으면 참이에요.
 * 실패: 없어요.
 */
    bool udp() const { return portName_.isEmpty(); }
/** 지원하는 시리얼 속도를 읽어요.
 * 입력: 없어요.
 * 출력: 지원하는 속도의 숫자 목록이에요.
 * 실패: 없어요.
 */
    QVariantList baudRates() const;
/** 고른 시리얼 속도를 읽어요.
 * 입력: 없어요.
 * 출력: 현재 고른 속도예요.
 * 실패: 없어요.
 */
    int baudRate() const { return baudRate_; }
/** 활성 기체 번호를 읽어요.
 * 입력: 없어요.
 * 출력: 활성 기체 번호이며 기체가 없으면 -1이에요.
 * 실패: 없어요.
 */
    int sysid() const;
/** 활성 기체의 펌웨어 이름을 읽어요.
 * 입력: 없어요.
 * 출력: 원본 capabilities의 이름이며 기체가 없으면 빈 값이에요.
 * 실패: 없어요.
 */
    QString firmwareName() const;
/** 활성 기체의 응답 유실 여부를 읽어요.
 * 입력: 없어요.
 * 출력: 활성 기체가 응답하지 않을 때만 참이에요.
 * 실패: 없어요.
 */
    bool lost() const;
/** 실패 문구를 현재 언어로 읽어요.
 * 입력: 없어요.
 * 출력: 현재 언어의 오류 문구예요.
 * 실패: 오류가 없으면 빈 값이에요.
 */
    Q_INVOKABLE QString failureText() const { return service_.failureText(); }
/** 목록 위치를 포트 이름으로 기억해요.
 * 입력: 고른 위치예요.
 * 출력: 포트 이름을 기억하고 portsChanged를 알려요.
 * 실패: 연결 중이거나 위치가 범위 밖이면 무시해요.
 * 흐름:
 * 1. LinkService::canConnect — 설정을 바꿀 수 있는 상태인지 확인해요.
 * 2. LinkService::ports — 원본 목록에서 고른 포트 이름을 찾아 기억해요.
 */
    Q_INVOKABLE void selectPort(int index);
/** 목록에서 속도를 골라요.
 * 입력: 고른 위치예요.
 * 출력: 선택한 속도를 바꾸고 baudRateChanged를 알려요.
 * 실패: 연결 중이거나 UDP이거나 위치가 범위 밖이면 무시해요.
 * 흐름:
 * 1. LinkService::canConnect — 설정을 바꿀 수 있는 상태인지 확인해요.
 * 2. kBaudRates — 지원 목록에서 선택한 속도를 기억해요.
 */
    Q_INVOKABLE void selectBaud(int index);
/** 현재 상태에 맞춰 연결하거나 끊어요.
 * 입력: 없어요.
 * 출력: 결과는 서비스의 연결 상태 변경으로 알려요.
 * 실패: 연결 오류는 Failed 상태로 제공해요.
 * 흐름:
 * 1. LinkService::canConnect — 새 연결을 시작할 상태인지 확인해요.
 * 2. LinkService::connectLink 또는 disconnectLink — 선택한 연결을 시작하거나 끝내요.
 */
    Q_INVOKABLE void toggleConnection();
signals:
/** 표시값을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: 변경 알림이에요.
 * 실패: 없어요.
 */
    void connectionStateChanged();
/** 표시값을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: 변경 알림이에요.
 * 실패: 없어요.
 */
    void portsChanged();
/** 표시값을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: 변경 알림이에요.
 * 실패: 없어요.
 */
    void baudRateChanged();
/** 표시값을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: 변경 알림이에요.
 * 실패: 없어요.
 */
    void vehicleChanged();
private:
    void rebind();
    void refreshPorts();
    LinkService& service_;
    QString portName_;
    int baudRate_;
    QPointer<kgc::vehicle::registry::Vehicle> vehicle_;
    inline static QPointer<LinkController> instance_;
};
}
