// [0.1.0 T9 T10 T11 T12 T13 T16 T17 T20] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/registry/VehicleRegistry.h>
#include <QQmlEngine>
#include <QPointer>
namespace kgc::vehicleInfo {
/** 활성 기체의 상태를 계기와 개요에 제공해요.
 * 입력: 기체 등록부예요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
class VehicleInfoController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool hasVehicle READ hasVehicle NOTIFY valuesChanged)
    Q_PROPERTY(bool available READ available NOTIFY valuesChanged)
    Q_PROPERTY(double roll READ roll NOTIFY valuesChanged)
    Q_PROPERTY(double pitch READ pitch NOTIFY valuesChanged)
    Q_PROPERTY(double heading READ heading NOTIFY valuesChanged)
    Q_PROPERTY(double speed READ speed NOTIFY valuesChanged)
    Q_PROPERTY(double altitude READ altitude NOTIFY valuesChanged)
    Q_PROPERTY(double altAmsl READ altAmsl NOTIFY valuesChanged)
    Q_PROPERTY(double climbRate READ climbRate NOTIFY valuesChanged)
    Q_PROPERTY(QString modeName READ modeName NOTIFY valuesChanged)
    Q_PROPERTY(qint64 modeId READ modeId NOTIFY valuesChanged)
    Q_PROPERTY(bool armed READ armed NOTIFY valuesChanged)
    Q_PROPERTY(double batteryVoltage READ batteryVoltage NOTIFY valuesChanged)
    Q_PROPERTY(double batteryCurrent READ batteryCurrent NOTIFY valuesChanged)
    Q_PROPERTY(double batteryPercent READ batteryPercent NOTIFY valuesChanged)
    Q_PROPERTY(int fixType READ fixType NOTIFY valuesChanged)
    Q_PROPERTY(double satellites READ satellites NOTIFY valuesChanged)
    Q_PROPERTY(bool udp READ udp NOTIFY valuesChanged)
    Q_PROPERTY(QString portName READ portName NOTIFY valuesChanged)
    Q_PROPERTY(int baudRate READ baudRate NOTIFY valuesChanged)
public:
/** 활성 기체가 바뀔 때 상태 신호를 다시 묶어요.
 * 입력: 기체 등록부이며 부모도 이 등록부예요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    explicit VehicleInfoController(kgc::vehicle::registry::VehicleRegistry& vehicles);
/** 등록된 컨트롤러를 QML에 제공해요.
 * 입력: QML 엔진과 스크립트 엔진이에요.
 * 출력: C++가 소유하는 컨트롤러예요.
 * 실패: 없어요.
 */
    static VehicleInfoController* create(QQmlEngine* engine, QJSEngine* scriptEngine);
/** 활성 기체가 있는지 알려요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    bool hasVehicle() const;
/** 응답 중인 활성 기체의 유한한 기울기 값이 있어 계기를 켤 수 있는지 알려요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    bool available() const;
/** 자세 묶음의 기울기를 도 단위로 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    double roll() const;
/** 자세 묶음의 기수 각도를 도 단위로 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    double pitch() const;
/** 위치의 선택적 방위 대신 자세 묶음의 기수 방위를 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    double heading() const;
/** 수평 속도를 초당 미터 단위로 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    double speed() const;
/** 기준점에 대한 상대 고도를 미터 단위로 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    double altitude() const;
/** 평균 해수면 기준 고도를 미터 단위로 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    double altAmsl() const;
/** 상승 속도를 초당 미터 단위로 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    double climbRate() const;
/** 펌웨어가 제공한 공식 모드 이름을 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    QString modeName() const;
/** 이름이 없는 모드의 표시를 위해 번호를 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    qint64 modeId() const;
/** 원본 시동 상태를 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    bool armed() const;
/** 전압을 볼트 단위로 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    double batteryVoltage() const;
/** 전류를 암페어 단위로 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    double batteryCurrent() const;
/** 수신하지 않은 잔량은 NaN으로 바꿔 계기가 빈 값으로 그리게 해요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    double batteryPercent() const;
/** QML이 번역된 이름으로 바꿀 GPS 수신 종류를 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    int fixType() const;
/** 수신하지 않은 위성 개수는 NaN으로 바꿔 빈 값으로 표시하게 해요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    double satellites() const;
/** 활성 기체를 발견한 통로가 UDP인지 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    bool udp() const;
/** 활성 기체를 발견한 시리얼 포트 이름을 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    QString portName() const;
/** 활성 기체를 발견한 시리얼 속도를 읽어요.
 * 입력: 없어요.
 * 출력: 원본 상태의 값이며 수신 전에는 기본값이에요.
 * 실패: 없어요.
 */
    int baudRate() const;
signals:
/** 계기와 개요 값을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: 변경 알림이에요.
 * 실패: 없어요.
 */
    void valuesChanged();
private:
    void rebind();
    kgc::vehicle::states::AttitudeBundle attitude() const;
    kgc::vehicle::states::PositionBundle position() const;
    kgc::vehicle::states::ArmingBundle arming() const;
    kgc::vehicle::states::BatteryBundle battery() const;
    kgc::vehicle::states::GpsBundle gps() const;
    kgc::vehicle::registry::VehicleRegistry& vehicles_;
    QPointer<kgc::vehicle::registry::Vehicle> vehicle_;
    inline static QPointer<VehicleInfoController> instance_;
};
}
