// [0.6.0 5.11.6] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/fly/FlyTypes.h>
#include <kgc/vehicle/states/VehicleStates.h>
#include <QObject>
#include <QString>
#include <optional>
#include <utility>
namespace kgc::app::scheduler { class Timeout; }
namespace kgc::fly {
/** 보낸 여기로 가기 목표의 확인 단계를 들고, 기체가 보내는 항법 목표로 받아들임·거부·바뀜을 판정해요.
 * 입력: 기체 상태(시동·모드·비행 상태·항법 목표·기체 메시지)와 FlyService의 begin·accept·watch·fail·clear 호출이에요.
 * 출력: 지금 목표 target과 changed, 거부 rejected, 지켜보기 끝 watchEnded 신호예요.
 * 실패: 없어요. 거부와 지움은 신호와 로그로 알려요.
 */
class GoToTracker : public QObject {
    Q_OBJECT
public:
/** 상태 신호를 잇는 추적기를 만들어요. 입력: 기체 상태 states와 부모예요. 출력: 목표 없는 추적기예요. 실패: 없어요. */
    GoToTracker(kgc::vehicle::states::VehicleStates& states, QObject* parent);
/** 지금 목표를 읽어요. 입력: 없어요. 출력: 목표와 단계이며 없으면 None이에요. 실패: 없어요. */
    GoToTarget target() const;
/** 새 목표를 보낸 것으로 둬요(점선 표식).
 * 입력: 위도·경도, 홈 기준 고도, 그 목표가 살아 있는 유도 모드 guidedMode예요.
 * 출력: 받아들여졌던 앞 목표는 previous_로 기억하고 새 목표를 Sending으로 둬요.
 * 실패: 없어요.
 */
    void begin(double latDeg, double lonDeg, double altitudeRelM, std::uint32_t guidedMode);
/** ACK와 모드 확인이 끝났음을 받아요. 입력: 없어요. 출력: Accepted로 두고 5초 동안 기체 목표를 기다려요. 실패: 없어요. */
    void accept();
/** 모드는 못 바꿨지만 기체가 목표를 받았는지 5초 더 봐요. 입력: 없어요. 출력: Sending 그대로 지켜봐요. 실패: 없어요. */
    void watch();
/** 보내기가 실패했음을 받아요. 입력: 없어요. 출력: 앞 목표가 아직 살아 있으면 되살리고, 아니면 지워요. 실패: 없어요. */
    void fail();
/** 목표를 지워요. 입력: 로그에 남길 까닭 cause예요. 출력: 목표가 있었으면 None으로 두고 changed를 내요. 실패: 없어요. */
    void clear(const QString& cause);
signals:
/** target()이 바뀌었음을 알려요. 입력: 없어요. 출력: 다시 읽도록 알려요. 실패: 없어요. */
    void changed();
/** 기체가 받아들인 뒤 다른 곳을 목표로 보냈음을 알려요. 입력: 까닭이 된 마지막 기체 메시지 reason이에요. 출력: 한 번이에요. 실패: 없어요. */
    void rejected(const QString& reason);
/** watch로 시작한 지켜보기가 끝났음을 알려요. 입력: 기체 목표가 보낸 곳과 맞았으면 참인 kept예요. 출력: 한 번이에요. 실패: 없어요. */
    void watchEnded(bool kept);
private:
    void receiveTarget();
    void receiveArming();
    void expire();
    void restore(GoToState state);
    kgc::vehicle::states::VehicleStates& states_;
    kgc::app::scheduler::Timeout* timer_;
    GoToTarget target_;
    std::uint32_t guidedMode_ = 0;
    std::optional<std::pair<GoToTarget, std::uint32_t>> previous_;
    int seenCount_ = 0;
    int mismatches_ = 0;
    std::pair<double, double> lastVehicleTarget_;
    bool farSeen_ = false;
    double farDistanceM_ = 0.0;
    QString reason_;
    bool watching_ = false;
};
}
