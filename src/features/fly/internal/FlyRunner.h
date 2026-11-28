// [0.6.0 §12 2] specs/features/0.6.0-fly.md
// [0.6.0 5.11.5] specs/features/0.6.0-fly.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <kgc/fly/Errors.h>
#include <kgc/fly/FlyTypes.h>
#include <kgc/vehicle/registry/Vehicle.h>
#include <QObject>
#include <QString>
#include <optional>
namespace kgc::app::scheduler { class Timeout; }
namespace kgc::vehicle::command { struct CommandResult; }
namespace kgc::fly {
/** 플러그인이 준 명령 단계를 차례로 보내고, 단계마다 ACK와 HEARTBEAT 확인을 기다려요.
 * 입력: 기체 vehicle의 명령 큐와 상태예요.
 * 출력: 실행마다 finished를 한 번 내요. 이유 없이 실패하면 1초(kReasonGrace) 안 늦게 온 기체 이유를 reasonArrived로 한 번 더 알려요.
 * 실패: ACK 거부·응답 없음·확인 시간 초과는 finished의 FlyError로 알려요.
 */
class FlyRunner : public QObject {
    Q_OBJECT
public:
/** 기체 메시지를 듣는 실행기를 만들어요. 입력: 기체와 부모예요. 출력: 쉬는 실행기예요. 실패: 없어요. */
    FlyRunner(kgc::vehicle::registry::Vehicle& vehicle, QObject* parent);
/** 실행 중인지 읽어요. 입력: 없어요. 출력: 마지막 단계가 끝나기 전이면 참이에요. 실패: 없어요. */
    bool busy() const;
/** 단계 목록 실행을 시작해요.
 * 입력: 동작 종류 kind, 단계 steps, 첫 단계를 큐에 끼어들어 보낼지 preempt예요.
 * 출력: 앞 실행이 남았으면 그것을 finished(앞 종류, Preempted)로 먼저 끝내고 첫 단계를 보내요. 앞 실행의 늦은 이유 듣기는 멈춰요.
 * 실패: 단계 결과는 finished로 알려요.
 */
    void start(FlyActionKind kind, QList<kgc::vehicle::firmware::CommandStep> steps, bool preempt);
signals:
/** 실행 하나가 끝났음을 알려요.
 * 입력: 동작 종류 kind, 결과 result, 실패 까닭이 된 마지막 기체 메시지 reason(성공·Preempted면 빈 값)이에요.
 * 출력: 실행마다 한 번이에요.
 * 실패: 없어요.
 */
    void finished(kgc::fly::FlyActionKind kind, kgc::app::errors::Result<void> result, const QString& reason);
/** 이유 없이 끝난 실패 뒤에 늦게 온 기체 이유를 알려요(ArduPilot은 시동 거부 이유를 ACK 뒤에 보내요).
 * 입력: 실패한 동작 종류 kind와 isReason이 참인 첫 기체 메시지 글 reason이에요.
 * 출력: Preempted가 아닌 실패로 끝났고 finished의 reason이 비었을 때, 끝난 뒤 kReasonGrace 안에 온 첫 이유로 한 번이에요.
 * 실패: 새 실행이 시작되거나 시간이 다 되면 듣기를 멈추고 내지 않아요.
 */
    void reasonArrived(kgc::fly::FlyActionKind kind, QString reason);
private:
    void sendStep();
    void receive(const kgc::vehicle::command::CommandResult& result);
    bool confirmed() const;
    void wait(std::optional<FlyError> error);
    void next();
    void finish(kgc::app::errors::Result<void> result);
    kgc::vehicle::registry::Vehicle& vehicle_;
    kgc::app::scheduler::Timeout* timeout_;
    kgc::app::scheduler::Timeout* reasonGrace_;
    int run_ = 0;
    bool active_ = false;
    FlyActionKind kind_ = FlyActionKind::Arm;
    QList<kgc::vehicle::firmware::CommandStep> steps_;
    qsizetype index_ = 0;
    bool preempt_ = false;
    bool waiting_ = false;
    std::optional<FlyError> waitError_;
    QString reason_;
    bool lateListening_ = false;
    FlyActionKind lateKind_ = FlyActionKind::Arm;
};
}
