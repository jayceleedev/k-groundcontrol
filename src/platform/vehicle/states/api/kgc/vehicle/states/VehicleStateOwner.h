// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QObject>
#include <QElapsedTimer>
struct __mavlink_message;
namespace kgc::app::scheduler { class Scheduler; }
namespace kgc::vehicle::firmware { class FirmwarePlugin; }
namespace kgc::vehicle::states {
class VehicleStates;
/** 수신 메시지와 끊김 검사를 상태 묶음에 반영하는 유일한 주인이에요.
 * 입력: 상태 저장소, 펌웨어 플러그인, 스케줄러와 등록부가 거른 기체 메시지예요.
 * 출력: 갱신한 상태 묶음과 VehicleStates의 변경 시그널이에요.
 * 실패: 지원하지 않는 메시지는 무시해요.
 */
class VehicleStateOwner : public QObject {
public:
/** 기체별 끊김 검사 작업을 등록해요.
 * 입력: 갱신할 states, 모드를 읽을 plugin, scheduler, 작업 이름에 붙일 sysid, 선택적인 parent예요.
 * 출력: 단조 시계와 기체별 1Hz 끊김 검사 작업을 준비해요.
 * 실패: 살아 있는 같은 sysid의 검사 작업이 이미 있으면 스케줄러가 프로그래밍 오류로 처리해요.
 */
    VehicleStateOwner(VehicleStates& states, const kgc::vehicle::firmware::FirmwarePlugin& plugin,
                      kgc::app::scheduler::Scheduler& scheduler, int sysid, QObject* parent = nullptr);
/** 등록부가 걸러 준 기체 메시지를 상태로 옮겨요.
 * 입력: 같은 기체의 자동조종기에서 받은 완성 메시지 message예요.
 * 출력: 해당 묶음을 갱신하고 변경 시그널을 내보내요.
 * 실패: 지원하지 않는 메시지는 상태를 바꾸지 않아요.
 */
    void consume(const __mavlink_message& message);
private:
    void checkLinkLoss();
    VehicleStates& states_;
    const kgc::vehicle::firmware::FirmwarePlugin& plugin_;
    QElapsedTimer clock_;
};
}
