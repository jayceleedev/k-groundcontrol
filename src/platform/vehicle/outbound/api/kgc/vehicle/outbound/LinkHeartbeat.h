// [0.1.1 T8 T9] specs/features/0.1.1-link-hardening.md
#pragma once
#include <QObject>
#include <QPointer>
namespace kgc::app::scheduler { class Scheduler; }
namespace kgc::comm::links { class LinkManager; }
namespace kgc::vehicle::outbound {
/** 기체를 찾기 전부터 링크마다 GCS 하트비트를 보내요.
 * 입력: 연결 관리자와 1Hz 작업을 등록할 스케줄러예요.
 * 출력: 링크가 열릴 때 한 번, 그 뒤 1초마다 MAVLink v2 하트비트를 송신 예약해요.
 * 실패: 열린 링크가 없으면 보내지 않고 조용히 넘어가요.
 */
class LinkHeartbeat : public QObject {
public:
/** 링크 열림 신호와 1Hz 작업을 연결해요.
 * 입력: links, scheduler, 선택적인 parent예요.
 * 출력: 앱이 끝날 때까지 하트비트를 맡아요.
 * 실패: 같은 스케줄러에 gcsHeartbeat 이름이 이미 있으면 프로그래밍 오류예요.
 */
    LinkHeartbeat(kgc::comm::links::LinkManager& links, kgc::app::scheduler::Scheduler& scheduler,
                  QObject* parent = nullptr);
private:
    void send();
    QPointer<kgc::comm::links::LinkManager> links_;
};
}
