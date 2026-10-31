// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/app/errors/Result.h>
#include <kgc/vehicle/outbound/OutboundKind.h>
#include <QObject>
#include <QPointer>
#include <functional>
namespace kgc::app::scheduler { class Scheduler; }
namespace kgc::comm::links { class LinkManager; }
namespace kgc::vehicle::firmware { class FirmwarePlugin; }
namespace kgc::vehicle::outbound {
/** 기체별 패킹과 링크 워커로의 송신을 담당해요.
 * 입력: 연결 관리자, 대상 기체·링크 번호, 스케줄러와 펌웨어의 스트림 목록이에요.
 * 출력: GCS 하트비트와 요청한 메시지를 MAVLink v2로 송신 예약해요.
 * 실패: 사용할 링크가 없으면 send가 NoLink를 돌려줘요.
 */
class VehicleSender : public QObject {
public:
    using PackFn = std::function<void(std::uint8_t, std::uint8_t, std::uint8_t, mavlink_message_t&)>;
/** 기체별 하트비트와 초기 스트림 요청을 시작해요.
 * 입력: links, scheduler, 대상 sysid·linkId, 수명 동안 유지되는 plugin, 선택적인 parent예요.
 * 출력: 1Hz 하트비트 작업을 등록하고 플러그인의 초기 스트림을 요청해요.
 * 실패: 같은 sysid의 작업 중복은 프로그래밍 오류이며 초기 스트림 예약 실패는 별도로 알리지 않아요.
 */
    VehicleSender(kgc::comm::links::LinkManager& links, kgc::app::scheduler::Scheduler& scheduler,
                  int sysid, int linkId, const kgc::vehicle::firmware::FirmwarePlugin& plugin,
                  QObject* parent = nullptr);
/** GCS 번호와 채널로 한 번 패킹해서 워커로 전달해요.
 * 입력: 송신 목적 kind와 GCS sysid·compid·채널·메시지 참조를 받는 pack 함수예요.
 * 출력: 패킹한 바이트의 워커 예약이 성공하면 성공 결과를 돌려줘요.
 * 실패: 대상 링크가 없거나 예약할 수 없으면 NoLink예요. 객체 스레드 밖 호출은 프로그래밍 오류예요.
 */
    kgc::app::errors::Result<void> send(OutboundKind kind, PackFn pack);
/** 처음 발견했거나 응답이 돌아온 기체에 스트림을 요청해요.
 * 입력: 생성 시 받은 플러그인의 스트림 목록과 대상 sysid를 사용해요.
 * 출력: 목록마다 REQUEST_DATA_STREAM을 예약하며 빈 목록은 아무것도 보내지 않아요.
 * 실패: 개별 송신의 NoLink 결과는 이 함수에서 따로 내보내지 않아요.
 */
    void sendStreamRequests();
private:
    void sendHeartbeat();
    QPointer<kgc::comm::links::LinkManager> links_;
    const kgc::vehicle::firmware::FirmwarePlugin& plugin_;
    int sysid_;
    int linkId_;
};
}
