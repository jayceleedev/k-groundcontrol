// [0.1.1 T6 T14 T17] specs/features/0.1.1-link-hardening.md
// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
#pragma once
#include <kgc/app/errors/ErrorCode.h>
#include <kgc/comm/links/LinkConfig.h>
#include <QObject>
#include <QElapsedTimer>
#include <QtQml/qqmlregistration.h>
#include <optional>
namespace kgc::app::scheduler { class Scheduler; }
namespace kgc::comm::links { class LinkManager; }
namespace kgc::vehicle::registry { class VehicleRegistry; }
namespace kgc::link {
/** 열린 링크에 기체가 너무 오래 없는지 판정해요.
 * 입력: 같은 단조 시계의 열린 시각 openedMs·현재 시각 nowMs와 기체 존재 여부 hasVehicle이에요.
 * 출력: 열린 시각이 0 이상이고 기체 없이 10000ms 이상 지났으면 참이에요.
 * 실패: 없어요. 열린 시각이 -1이면 거짓이에요.
 */
bool noVehicleTooLong(qint64 openedMs, qint64 nowMs, bool hasVehicle);
/** 연결 요청과 비동기 결과를 화면 상태로 모아요.
 * 입력: 링크 관리자와 기체 등록부, 검사 작업을 등록할 스케줄러예요.
 * 출력: 연결 상태와 실패 원인, 기체 없음 안내 여부예요.
 * 실패: 없어요.
 */
class LinkService : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Created by Composition")
public:
/** 연결 요청이 비동기 결과를 기다리는 단계를 구분해요.
 * 입력: 관리자의 열기·실패·종료 신호예요.
 * 출력: 화면이 비교할 네 상태예요.
 * 실패: 없어요.
 */
    enum class ConnectionState { Idle, Opening, Open, Failed };
    Q_ENUM(ConnectionState)
/** 연결의 시작과 끝 신호를 구독해요.
 * 입력: 수명 동안 유지되는 관리자·등록부·scheduler와 부모예요.
 * 출력: 1Hz 검사와 연결·활성 기체 변경 구독을 준비해요.
 * 실패: linkNoVehicle 작업 이름이 겹치면 프로그래밍 오류예요.
 */
    LinkService(kgc::comm::links::LinkManager& links,
        kgc::vehicle::registry::VehicleRegistry& vehicles, kgc::app::scheduler::Scheduler& scheduler,
        QObject* parent = nullptr);
/** 현재 연결 상태를 읽어요.
 * 입력: 없어요.
 * 출력: Idle·Opening·Open·Failed 중 하나예요.
 * 실패: 없어요.
 */
    ConnectionState connectionState() const { return state_; }
/** 연결 설정을 바꾸거나 새 연결을 시작할 수 있는지 판단해요.
 * 입력: 없어요.
 * 출력: Idle 또는 Failed 상태이면 참이에요.
 * 실패: 없어요.
 */
    bool canConnect() const;
/** 기체 없음 안내가 필요한지 읽어요.
 * 입력: 없어요.
 * 출력: 열린 링크에서 10초 동안 활성 기체가 없으면 참이에요.
 * 실패: 없어요.
 */
    bool noVehicle() const { return noVehicle_; }
/** 현재 연결 값의 통로 종류를 읽어요.
 * 입력: 없어요.
 * 출력: 관리자가 가진 연결 값이 시리얼이면 참이에요.
 * 실패: 없어요.
 */
    bool serialLink() const;
/** 관리자가 가진 포트 목록을 읽어요.
 * 입력: 없어요.
 * 출력: 포트 이름과 설명의 쌍이에요.
 * 실패: 없어요.
 */
    QList<QPair<QString, QString>> ports() const;
/** 활성 기체를 찾을 등록부를 제공해요.
 * 입력: 없어요.
 * 출력: 비소유 참조예요.
 * 실패: 없어요.
 */
    kgc::vehicle::registry::VehicleRegistry& vehicles() const { return vehicles_; }
/** 실패 원인을 현재 언어로 읽어요.
 * 입력: 없어요.
 * 출력: 실패 문구이며 오류가 없으면 빈 값이에요.
 * 실패: 없어요.
 */
    QString failureText() const;
/** 선택한 통로의 연결을 시작해요.
 * 입력: 연결 값이에요.
 * 출력: 없어요.
 * 실패: 관리자의 오류는 Failed 상태로 알려요.
 * 흐름:
 * 1. canConnect — 새 연결을 시작할 수 있는 상태인지 확인해요.
 * 2. LinkManager::connectLink — 워커에 열기를 요청하고 결과 신호를 기다려요.
 */
    void connectLink(const kgc::comm::links::LinkConfig& config);
/** 진행 중인 연결을 취소하거나 닫아요.
 * 입력: 없어요.
 * 출력: 없어요.
 * 실패: 없어요.
 * 흐름:
 * 1. LinkManager::disconnectLink — 닫기를 요청하고 linkClosed가 오면 상태를 바꿔요.
 */
    void disconnectLink();
signals:
/** 기체 없음 안내 여부가 바뀌었음을 알려요.
 * 입력: 없어요.
 * 출력: noVehicle을 다시 읽도록 알리는 시그널이에요.
 * 실패: 없어요.
 */
    void noVehicleChanged();
/** 연결 상태와 실패 원인을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: 없어요.
 * 실패: 없어요.
 */
    void connectionStateChanged();
/** 포트 목록을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: 없어요.
 * 실패: 없어요.
 */
    void portsChanged();
private:
    void checkNoVehicle();
    QElapsedTimer clock_;
    qint64 openedMs_ = -1;
    bool noVehicle_ = false;
    void finish(std::optional<kgc::app::errors::ErrorCode> error);
    kgc::comm::links::LinkManager& links_;
    kgc::vehicle::registry::VehicleRegistry& vehicles_;
    ConnectionState state_ = ConnectionState::Idle;
    std::optional<kgc::app::errors::ErrorCode> error_;
};
}
