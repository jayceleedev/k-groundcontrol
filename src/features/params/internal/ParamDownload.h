// [0.3.0 5.5 6.2 6.8-1 T5] specs/features/0.3.0-params.md
#pragma once
#include <kgc/app/errors/ErrorCode.h>
#include <kgc/app/job/Job.h>
#include <QHash>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QString>
namespace kgc::app::scheduler { class Timeout; }
namespace kgc::vehicle::registry { class Vehicle; }
namespace kgc::params {
/** 다시 요청할 번호를 골라요.
 * 입력: 전체 수 total, 받은 번호 received, 번호별 시도 수 tries이며 tries는 고른 번호만큼 늘어나요.
 * 출력: 번호 0부터 안 받은 것 중 시도가 남은 것을 최대 kParamReadBatch개 돌려줘요.
 * 실패: 고를 것이 없으면 빈 목록이에요.
 */
QList<int> nextRequests(int total, const QSet<int>& received, QHash<int, int>& tries);
/** 파라미터 목록 내려받기 한 회차를 끝까지 몰아가요.
 * 입력: 기체와, ParamService가 넘기는 Job과 PARAM_VALUE예요.
 * 출력: 끝을 completed·failed·cancelled로 알려요. Job의 진행률을 바꾸지만 끝맺음은 ParamService가 해요.
 * 실패: 응답이 없으면 failed(Timeout), 끝내 빠진 번호가 있으면 failed(Incomplete)예요.
 */
class ParamDownload : public QObject {
    Q_OBJECT
public:
/** 멈춘 내려받기를 만들어요.
 * 입력: 수명 동안 유지되는 vehicle과 parent예요.
 * 출력: start 전까지 아무것도 보내지 않아요.
 * 실패: 없어요.
 */
    ParamDownload(kgc::vehicle::registry::Vehicle& vehicle, QObject* parent);
/** 새 회차를 시작해요.
 * 입력: 진행을 보일 job이에요.
 * 출력: 받은 번호를 비우고 PARAM_REQUEST_LIST를 보내요.
 * 실패: 없어요.
 */
    void start(kgc::app::job::Job* job);
/** 지금 내려받는 중인지 읽어요. 입력: 없어요. 출력: 회차 중이면 참이에요. 실패: 없어요. */
    bool active() const;
/** 받은 PARAM_VALUE를 셈해요.
 * 입력: 이름 name, param_index인 index, param_count인 count예요.
 * 출력: 받은 수가 전체가 되면 completed를 내요.
 * 실패: 회차 중이 아니면 무시해요.
 */
    void receive(const QString& name, int index, int count);
/** 이번 회차에 받은 수를 읽어요. 입력: 없어요. 출력: 받은 번호 수예요. 실패: 없어요. */
    int received() const;
/** FC가 알린 전체 수를 읽어요. 입력: 없어요. 출력: 전체 수예요. 실패: 없어요. */
    int total() const;
signals:
/** 전부 받았음을 알려요. 입력: 받은 이름 names예요. 출력: 표 정리에 써요. 실패: 없어요. */
    void completed(const QSet<QString>& names);
/** 실패로 끝났음을 알려요. 입력: 오류 error예요. 출력: Job 실패에 써요. 실패: 없어요. */
    void failed(kgc::app::errors::ErrorCode error);
/** 사용자가 취소했음을 알려요. 입력: 없어요. 출력: Job 취소에 써요. 실패: 없어요. */
    void cancelled();
private:
    void requestList();
    void listExpired();
    void gapExpired();
    void finish();
    kgc::vehicle::registry::Vehicle& vehicle_;
    kgc::app::scheduler::Timeout* timer_;
    QPointer<kgc::app::job::Job> job_;
    QSet<int> received_;
    QSet<QString> names_;
    QHash<int, int> tries_;
    int total_ = 0;
    int listRetries_ = 0;
    bool started_ = false;
};
}
