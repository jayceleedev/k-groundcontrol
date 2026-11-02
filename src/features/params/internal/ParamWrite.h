// [0.3.0 6.6 6.8-1] specs/features/0.3.0-params.md
#pragma once
#include <kgc/app/job/Job.h>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <cstdint>
namespace kgc::app::scheduler { class Timeout; }
namespace kgc::vehicle::registry { class Vehicle; }
namespace kgc::params {
/** FC에 쓸 값 하나예요.
 * 입력: 이름, 쓸 값, FC가 보낸 형이에요.
 * 출력: ParamWrite::start의 줄이에요.
 * 실패: 없어요.
 */
struct ParamWriteItem { QString name; double value; std::uint8_t type; };
/** 대기값을 하나씩 PARAM_SET으로 쓰고 돌아온 값을 확인해요.
 * 입력: 기체와, ParamService가 넘기는 Job·줄 목록·PARAM_VALUE예요.
 * 출력: 성공한 줄마다 written을 내고, 끝나면 ended 뒤에 Job을 끝내요.
 * 실패: 줄 실패는 ended의 failed로, 시동 중단은 Job 실패(Armed)로 알려요.
 */
class ParamWrite : public QObject {
    Q_OBJECT
public:
/** 멈춘 쓰기를 만들어요.
 * 입력: 수명 동안 유지되는 vehicle과 parent예요.
 * 출력: start 전까지 아무것도 보내지 않아요.
 * 실패: 없어요.
 */
    ParamWrite(kgc::vehicle::registry::Vehicle& vehicle, QObject* parent);
/** 쓰기를 시작해요.
 * 입력: 끝맺을 job, 이름순 줄 items, 일괄이면 참인 bulk예요.
 * 출력: 첫 줄의 PARAM_SET을 보내요.
 * 실패: 없어요.
 */
    void start(kgc::app::job::Job* job, QList<ParamWriteItem> items, bool bulk);
/** 받은 PARAM_VALUE를 지금 줄과 맞춰 봐요.
 * 입력: 이름 name과 값 value예요.
 * 출력: 같으면 다음 줄로, 다르면 다음 시도로 가요.
 * 실패: 지금 줄이 아니면 무시해요.
 */
    void receive(const QString& name, double value);
signals:
/** 한 줄을 썼음을 알려요. 입력: 이름 name이에요. 출력: 재부팅 표시에 써요. 실패: 없어요. */
    void written(const QString& name);
/** 쓰기가 끝났음을 알려요.
 * 입력: 쓴 수 count와 쓰지 못한 이름 failed예요.
 * 출력: Job을 끝내기 바로 앞에 나와요. 기체가 사라졌을 때는 나오지 않아요.
 * 실패: 없어요.
 */
    void ended(int count, const QStringList& failed);
private:
    void next();
    void transmit();
    void retry();
    void expired();
    void advance();
    void release();
    kgc::vehicle::registry::Vehicle& vehicle_;
    kgc::app::scheduler::Timeout* timer_;
    QPointer<kgc::app::job::Job> job_;
    QList<ParamWriteItem> items_;
    QStringList failed_;
    qsizetype current_ = 0;
    int count_ = 0;
    int attempts_ = 0;
    bool bulk_ = false;
    bool cancelRequested_ = false;
};
}
