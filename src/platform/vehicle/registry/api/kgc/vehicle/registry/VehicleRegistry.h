// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/vehicle/registry/Vehicle.h>
#include <QObject>
#include <QPointer>
#include <QMap>
namespace kgc::vehicle::registry {
/** 링크가 발견한 기체와 활성 기체를 관리해요.
 * 입력: LinkManager의 수신·종료 시그널과 기체 작업용 스케줄러예요.
 * 출력: sysid로 구분한 기체 목록과 활성 기체, 추가·제거·활성 변경 시그널이에요.
 * 실패: 기체 발견 조건에 맞지 않는 하트비트는 무시해요.
 */
class VehicleRegistry : public QObject {
    Q_OBJECT
    Q_PROPERTY(kgc::vehicle::registry::Vehicle* active READ active NOTIFY activeChanged)
public:
/** 링크 수신과 종료 시그널에 기체 등록부를 연결해요.
 * 입력: 수명 동안 유지되는 links·scheduler와 선택적인 parent예요.
 * 출력: 수신에 따라 기체를 만들고 링크 종료 때 제거하는 등록부예요.
 * 실패: 없어요.
 */
    VehicleRegistry(kgc::comm::links::LinkManager& links, kgc::app::scheduler::Scheduler& scheduler,
                    QObject* parent = nullptr);
/** 현재 활성 기체를 읽어요.
 * 입력: 없어요.
 * 출력: 활성 Vehicle의 비소유 포인터이며, 기체가 없으면 nullptr예요.
 * 실패: 없어요.
 */
    Vehicle* active() const;
/** 시스템 번호 순서로 기체의 안전한 참조를 읽어요.
 * 입력: 없어요.
 * 출력: sysid 오름차순의 QPointer 목록이며, 기체가 지워지면 참조도 비워져요.
 * 실패: 없어요.
 */
    QList<QPointer<Vehicle>> vehicles() const;
signals:
/** 첫 상태가 준비된 새 기체를 알려요.
 * 입력: 첫 HEARTBEAT를 반영한 vehicle 포인터예요.
 * 출력: 등록부가 소유하는 기체의 추가 알림이며 소유권은 넘기지 않아요.
 * 실패: 없어요.
 */
    void vehicleAdded(kgc::vehicle::registry::Vehicle* vehicle);
/** 지연 삭제 전에 기체 제거를 알려요.
 * 입력: 반복 작업을 이미 멈춘 vehicle 포인터예요.
 * 출력: 기체 제거 알림이며, 수신자는 참조를 해제하고 실제 삭제는 deleteLater로 이어져요.
 * 실패: 없어요.
 */
    void vehicleRemoved(kgc::vehicle::registry::Vehicle* vehicle);
/** 활성 기체가 바뀌었음을 알려요.
 * 입력: 없어요.
 * 출력: active를 다시 읽도록 알리며, 남은 기체가 없으면 nullptr가 돼요.
 * 실패: 없어요.
 */
    void activeChanged();
private:
    void receive(int linkId, const __mavlink_message& message);
    void remove(int sysid);
    kgc::comm::links::LinkManager& links_;
    kgc::app::scheduler::Scheduler& scheduler_;
    QMap<int, QPointer<Vehicle>> vehicles_;
    QPointer<Vehicle> active_;
};
}
