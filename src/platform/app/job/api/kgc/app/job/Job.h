// [0.2.0 T2] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <kgc/app/job/OperationKind.h>
#include <QObject>
#include <QString>
#include <functional>
#include <memory>
#include <optional>
namespace kgc::app::job {
/** 오래 걸리는 작업 하나의 상태를 화면과 호출자에게 내놓아요.
 * 입력: 작업을 돌리는 모듈이 부르는 단계·진행률·끝맺음 함수예요.
 * 출력: state·stepName·progress·etaSeconds·cancellable·errorCode·errorText 프로퍼티와 finished 시그널이에요.
 * 실패: 끝난 작업에 다시 끝맺음을 불러도 무시하고 finished는 한 번만 나와요.
 */
class Job : public QObject {
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString stepName READ stepName NOTIFY stepNameChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(int etaSeconds READ etaSeconds NOTIFY progressChanged)
    Q_PROPERTY(bool cancellable READ cancellable NOTIFY cancellableChanged)
    Q_PROPERTY(std::optional<kgc::app::errors::ErrorCode> errorCode READ errorCode NOTIFY stateChanged)
    Q_PROPERTY(QString errorText READ errorText NOTIFY stateChanged)
public:
/** 작업의 다섯 상태를 나타내요.
 * 입력: 없어요.
 * 출력: Pending·Running·Succeeded·Failed·Cancelled 중 하나예요.
 * 실패: 없어요.
 */
    enum class State { Pending, Running, Succeeded, Failed, Cancelled };
    Q_ENUM(State)
    /** 끝날 때 한 번 불리는, 이동만 되는 해제 핸들이에요. */
    using Releaser = std::move_only_function<void()>;
/** 대기 상태의 작업을 만들어요.
 * 입력: 작업 종류 kind와 선택적인 parent예요.
 * 출력: state는 Pending, progress와 etaSeconds는 -1, cancellable은 거짓인 작업이에요.
 * 실패: 없어요.
 */
    explicit Job(OperationKind kind, QObject* parent = nullptr);
/** 끝나지 않은 작업도 쥔 해제 핸들을 놓고 지워져요.
 * 입력: 없어요.
 * 출력: 남은 해제 핸들을 역순으로 불러요.
 * 실패: 없어요.
 */
    ~Job() override;
/** 작업 종류를 읽어요.
 * 입력: 없어요.
 * 출력: 만들 때 받은 OperationKind예요.
 * 실패: 없어요.
 */
    OperationKind kind() const;
/** 현재 상태를 읽어요.
 * 입력: 없어요.
 * 출력: 현재 State예요.
 * 실패: 없어요.
 */
    State state() const;
/** 단계 이름의 번역 원문 키를 읽어요.
 * 입력: 없어요.
 * 출력: 마지막 setStep의 키이며, 아직 없으면 빈 문자열이에요. JobView가 qsTranslate("Job", key)로 그려요.
 * 실패: 없어요.
 */
    QString stepName() const;
/** 진행률을 읽어요.
 * 입력: 없어요.
 * 출력: 0~1의 값이며, 모르면 -1이에요.
 * 실패: 없어요.
 */
    double progress() const;
/** 남은 시간을 읽어요.
 * 입력: 없어요.
 * 출력: 시작 뒤 경과와 진행률로 셈한 초이며, 모르거나 진행률이 1% 미만이면 -1이에요.
 * 실패: 없어요.
 */
    int etaSeconds() const;
/** 지금 취소할 수 있는지 읽어요.
 * 입력: 없어요.
 * 출력: 마지막 setCancellable의 값이에요.
 * 실패: 없어요.
 */
    bool cancellable() const;
/** 실패 코드를 읽어요.
 * 입력: 없어요.
 * 출력: fail 또는 finishCancelled의 코드이며, 그 밖에는 빈 값이에요.
 * 실패: 없어요.
 */
    std::optional<kgc::app::errors::ErrorCode> errorCode() const;
/** 실패 글자를 읽어요.
 * 입력: 없어요.
 * 출력: fail이 받은 글자이며, 없으면 빈 문자열이에요.
 * 실패: 없어요.
 */
    QString errorText() const;
/** 단계를 바꾸고, 대기 중이면 작업을 시작해요.
 * 입력: QT_TRANSLATE_NOOP("Job", …)로 표시한 번역 원문 key예요.
 * 출력: stepName을 바꿔요. 첫 호출이 state를 Running으로 바꾸고 ETA 시계를 시작해요.
 * 실패: 끝난 작업이면 무시해요.
 */
    void setStep(const QString& key);
/** 진행률을 바꾸고 남은 시간을 다시 셈해요.
 * 입력: 0~1의 value이며, 모르면 -1이에요.
 * 출력: progress와 etaSeconds를 바꿔요.
 * 실패: 끝난 작업이면 무시해요.
 */
    void setProgress(double value);
/** 취소 단추를 보일지 정해요.
 * 입력: 취소할 수 있으면 참인 value예요.
 * 출력: cancellable을 바꿔요.
 * 실패: 끝난 작업이면 무시해요.
 */
    void setCancellable(bool value);
/** 작업을 성공으로 끝내요.
 * 입력: 없어요.
 * 출력: state를 Succeeded로 바꾸고 해제 핸들을 역순으로 부른 뒤 finished(성공)를 한 번 내요.
 * 실패: 이미 끝났으면 무시해요.
 */
    void succeed();
/** 작업을 실패로 끝내요.
 * 입력: 오류 code와 화면에 보일 text예요.
 * 출력: state를 Failed로, errorCode·errorText를 채우고 해제 핸들을 역순으로 부른 뒤 finished(오류)를 한 번 내요.
 * 실패: 이미 끝났으면 무시해요.
 */
    void fail(kgc::app::errors::ErrorCode code, const QString& text);
/** 취소를 요청해요.
 * 입력: 없어요.
 * 출력: cancelRequested 시그널만 내요. 상태는 바꾸지 않고, 멈출 수 있는 곳에서 작업이 finishCancelled를 불러요.
 * 실패: 없어요.
 */
    void cancel();
/** 작업을 취소로 끝내요.
 * 입력: 없어요.
 * 출력: state를 Cancelled로 바꾸고 해제 핸들을 역순으로 부른 뒤 finished(JobError::Cancelled)를 한 번 내요.
 * 실패: 이미 끝났으면 무시해요.
 */
    void finishCancelled();
/** 끝날 때 놓을 자원을 맡겨요.
 * 입력: 이동만 되는 해제 핸들 releaser예요.
 * 출력: 끝날 때 맡긴 순서의 역순으로 한 번씩 불러요.
 * 실패: 없어요.
 */
    void hold(Releaser releaser);
signals:
/** 상태가 바뀌었음을 알려요.
 * 입력: 없어요.
 * 출력: state·errorCode·errorText를 다시 읽도록 알려요.
 * 실패: 없어요.
 */
    void stateChanged();
/** 단계가 바뀌었음을 알려요.
 * 입력: 없어요.
 * 출력: stepName을 다시 읽도록 알려요.
 * 실패: 없어요.
 */
    void stepNameChanged();
/** 진행률이 바뀌었음을 알려요.
 * 입력: 없어요.
 * 출력: progress와 etaSeconds를 다시 읽도록 알려요.
 * 실패: 없어요.
 */
    void progressChanged();
/** 취소 가능 여부가 바뀌었음을 알려요.
 * 입력: 없어요.
 * 출력: cancellable을 다시 읽도록 알려요.
 * 실패: 없어요.
 */
    void cancellableChanged();
/** 취소 요청을 작업을 돌리는 쪽에 알려요.
 * 입력: 없어요.
 * 출력: 받는 쪽이 멈출 수 있는 곳에서 finishCancelled를 불러요.
 * 실패: 없어요.
 */
    void cancelRequested();
/** 작업이 끝났음을 한 번 알려요.
 * 입력: 성공이면 값 없는 결과, 실패·취소면 오류 코드인 result예요.
 * 출력: 작업마다 정확히 한 번 나와요.
 * 실패: 없어요.
 */
    void finished(kgc::app::errors::Result<void> result);
private:
    struct Private;
    std::unique_ptr<Private> d_;
};
}
