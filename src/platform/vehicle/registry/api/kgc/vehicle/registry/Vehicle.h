// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/comm/links/LinkConfig.h>
#include <kgc/vehicle/states/VehicleStates.h>
#include <QObject>
#include <memory>
#include <cstdint>
struct __mavlink_message;
namespace kgc::app::scheduler { class Scheduler; }
namespace kgc::comm::links { class LinkManager; }
namespace kgc::vehicle::registry {
class VehicleRegistry;
/** 발견한 기체의 상태와 생성 당시 연결 값을 소유해요.
 * 입력: 등록부가 확인한 sysid·autopilot과 생성 당시 링크 정보예요.
 * 출력: 기체별 읽기 전용 상태와 연결 값이에요.
 * 실패: 없어요.
 */
class Vehicle : public QObject {
    Q_OBJECT
    Q_PROPERTY(int sysid READ sysid CONSTANT)
    Q_PROPERTY(kgc::vehicle::states::VehicleStates* states READ states CONSTANT)
public:
/** 반복 작업을 먼저 끝내고 기체 자원을 정리해요.
 * 입력: 없어요.
 * 출력: 송신·상태 소유자와 자식 객체를 해제해요.
 * 실패: 없어요.
 */
    ~Vehicle() override;
/** 기체 시스템 번호를 읽어요.
 * 입력: 없어요.
 * 출력: 처음 발견한 HEARTBEAT의 sysid이며 객체 수명 동안 고정돼요.
 * 실패: 없어요.
 */
    int sysid() const;
/** 기체 상태를 읽기 전용 경계로 내놓아요.
 * 입력: 없어요.
 * 출력: 기체가 소유하는 VehicleStates 포인터이며 소유권은 넘기지 않아요.
 * 실패: 없어요.
 */
    kgc::vehicle::states::VehicleStates* states() const;
/** 생성 당시의 연결 값을 복사해요.
 * 입력: 없어요.
 * 출력: 기체를 처음 발견한 링크의 LinkConfig 복사본이에요.
 * 실패: 없어요.
 */
    kgc::comm::links::LinkConfig linkConfig() const;
private:
    friend class VehicleRegistry;
    Vehicle(int sysid, std::uint8_t autopilot, kgc::comm::links::LinkManager& links,
            kgc::app::scheduler::Scheduler& scheduler, QObject* parent);
    void consume(const __mavlink_message& message);
    void stop();
    std::uint8_t autopilot() const;
    struct Private;
    std::unique_ptr<Private> d_;
};
}
