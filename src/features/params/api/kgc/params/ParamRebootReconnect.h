// [0.3.0 6.8-2] specs/features/0.3.0-params.md
#pragma once
#include <kgc/comm/links/LinkConfig.h>
#include <QObject>
namespace kgc::app::scheduler { class Timeout; }
namespace kgc::comm::links { class LinkManager; }
namespace kgc::vehicle::registry { class VehicleRegistry; }
namespace kgc::params {
/** 앱이 보낸 재부팅으로 USB 포트가 사라졌을 때 링크를 스스로 다시 열어요.
 * 입력: LinkManager의 끝 신호와 VehicleRegistry의 vehicleAdded, ParamService가 부르는 watch예요.
 * 출력: 감시 창 안에서 같은 연결 값으로 LinkManager::connectLink를 불러요.
 * 실패: 창이 끝나도 기체가 없으면 그대로 두고, 사용자가 연결해요.
 * 흐름:
 * 1. watch — 30초 창을 열어요.
 * 2. linkClosed(오류)·linkFailed — 6초 뒤 connectLink를 시도해요.
 * 3. 실패하거나 다시 닫히면 1초 뒤 다시 시도해요.
 * 4. vehicleAdded·사용자 끊기·창 끝 — 감시를 마쳐요.
 */
class ParamRebootReconnect : public QObject {
    Q_OBJECT
public:
/** 멈춘 감시를 만들어요.
 * 입력: 수명 동안 유지되는 links·vehicles와 parent예요.
 * 출력: watch 전까지 아무 일도 하지 않아요.
 * 실패: 없어요.
 */
    ParamRebootReconnect(kgc::comm::links::LinkManager& links, kgc::vehicle::registry::VehicleRegistry& vehicles,
                         QObject* parent);
/** 재부팅 뒤 감시 창을 열어요.
 * 입력: 재부팅한 기체의 연결 값 config예요.
 * 출력: 창이 열려요. 링크가 이미 끝났으면 다시 열기를 바로 예약해요.
 * 실패: 없어요.
 */
    void watch(const kgc::comm::links::LinkConfig& config);
private:
    void linkEnded();
    void attempt();
    void stop();
    kgc::comm::links::LinkManager& links_;
    kgc::app::scheduler::Timeout* window_;
    kgc::app::scheduler::Timeout* retry_;
    kgc::comm::links::LinkConfig config_;
    bool watching_ = false;
    bool attempted_ = false;
};
}
