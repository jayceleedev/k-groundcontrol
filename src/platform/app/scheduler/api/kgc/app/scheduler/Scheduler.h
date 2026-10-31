// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QObject>
#include <QHash>
#include <QPointer>
#include <chrono>
#include <functional>
namespace kgc::app::scheduler {
/** 이름 있는 반복 작업을 소유자의 수명에 묶어요.
 * 입력: 작업 이름·주기·소유자·실행 문맥·실행 함수예요.
 * 출력: 소유자가 살아 있는 동안 실행 문맥의 스레드에서 함수를 호출해요.
 * 실패: 중복 이름, 잘못된 인자, 소유자와 문맥의 스레드 불일치는 프로그래밍 오류예요.
 */
class Scheduler : public QObject {
public:
/** 반복 작업의 이름과 소유자를 관리할 등록부를 만들어요.
 * 입력: 수명을 함께할 부모 객체이며, 없으면 호출자가 소유해요.
 * 출력: 비어 있는 작업 등록부예요.
 * 실패: 없어요.
 */
    explicit Scheduler(QObject* parent = nullptr);
/** 작업을 지정한 스레드에서 주기적으로 실행해요.
 * 입력: 고유한 name, 양수인 period, 같은 스레드에 있는 owner·context, 호출할 fn이에요.
 * 출력: 반환값 없이 작업을 예약하고, owner나 스케줄러가 사라지면 호출을 멈춰요.
 * 실패: 스케줄러 스레드 밖 호출, 빈 객체·함수, 양수가 아닌 주기, 살아 있는 이름의 중복, 스레드 불일치는 프로그래밍 오류예요.
 */
    void registerTask(QString name, std::chrono::milliseconds period, QObject* owner,
                      QObject* context, std::function<void()> fn);
private:
    QHash<QString, QPointer<QObject>> owners_;
};
}
