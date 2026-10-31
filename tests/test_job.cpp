// [0.2.0 T2] specs/features/0.2.0-fcflash.md
#include <kgc/app/job/Job.h>
#include <kgc/app/job/Errors.h>
#include <QtTest>
#include <vector>
using kgc::app::errors::ErrorCode;
using kgc::app::errors::Result;
using kgc::app::job::Job;
using kgc::app::job::JobError;
using kgc::app::job::OperationKind;
class JobTest : public QObject {
    Q_OBJECT
private slots:
    /** T2: 새 작업은 대기 상태이고 진행률과 남은 시간을 몰라요. */
    void test_T2_initial_values()
    {
        Job job(OperationKind::FcFlash);
        QCOMPARE(job.kind(), OperationKind::FcFlash);
        QCOMPARE(job.state(), Job::State::Pending);
        QCOMPARE(job.progress(), -1.0);
        QCOMPARE(job.etaSeconds(), -1);
        QVERIFY(!job.cancellable());
        QVERIFY(!job.errorCode().has_value());
    }
    /** T2: 성공은 finished를 한 번만 내고, 뒤의 끝맺음은 무시해요. */
    void test_T2_finished_only_once()
    {
        Job job(OperationKind::FcFlash);
        int finished = 0;
        std::optional<Result<void>> last;
        connect(&job, &Job::finished, this, [&](Result<void> result) { ++finished; last = result; });
        job.setStep("쓰기");
        job.succeed();
        job.succeed();
        job.fail(ErrorCode{{7}, 1}, "x");
        job.finishCancelled();
        QCOMPARE(finished, 1);
        QVERIFY(last.has_value() && last->has_value());
        QCOMPARE(job.state(), Job::State::Succeeded);
        QVERIFY(!job.errorCode().has_value());
    }
    /** T2: 실패는 코드와 글자를 남기고 finished에 오류를 실어요. */
    void test_T2_fail_keeps_error()
    {
        Job job(OperationKind::FcFlash);
        const ErrorCode code{{7}, 3};
        int finished = 0;
        std::optional<Result<void>> last;
        connect(&job, &Job::finished, this, [&](Result<void> result) { ++finished; last = result; });
        job.setStep("쓰기");
        job.fail(code, "bad");
        QCOMPARE(finished, 1);
        QVERIFY(last.has_value() && !last->has_value());
        QVERIFY(last->error() == code);
        QCOMPARE(job.state(), Job::State::Failed);
        QVERIFY(job.errorCode() == code);
        QCOMPARE(job.errorText(), QString("bad"));
    }
    /** T2: 해제 핸들은 맡긴 순서의 역순으로 한 번씩 불려요. */
    void test_T2_hold_releases_in_reverse()
    {
        Job job(OperationKind::FcFlash);
        QList<int> order;
        job.hold([&order] { order << 1; });
        job.hold([&order] { order << 2; });
        job.hold([&order] { order << 3; });
        job.setStep("쓰기");
        job.succeed();
        QCOMPARE(order, QList<int>({3, 2, 1}));
        job.finishCancelled();
        QCOMPARE(order, QList<int>({3, 2, 1}));
    }
    /** T2: 해제 핸들은 이동만 되는 호출 가능 객체도 받아요. */
    void test_T2_hold_accepts_move_only()
    {
        Job job(OperationKind::FcFlash);
        int released = 0;
        auto token = std::make_unique<int>(5);
        job.hold([&released, token = std::move(token)] { released = *token; });
        job.setStep("쓰기");
        job.fail(ErrorCode{{7}, 1}, "x");
        QCOMPARE(released, 5);
    }
    /** T2: 실패로 끝나도 해제 핸들을 역순으로 불러요. */
    void test_T2_hold_releases_on_failure()
    {
        Job job(OperationKind::FcFlash);
        QList<int> order;
        job.hold([&order] { order << 1; });
        job.hold([&order] { order << 2; });
        job.setStep("쓰기");
        job.fail(ErrorCode{{7}, 1}, "x");
        QCOMPARE(order, QList<int>({2, 1}));
    }
    /** T2: cancel은 cancelRequested만 내고 상태를 바꾸지 않아요. */
    void test_T2_cancel_only_requests()
    {
        Job job(OperationKind::FcFlash);
        int requested = 0;
        int finished = 0;
        connect(&job, &Job::cancelRequested, this, [&] { ++requested; });
        connect(&job, &Job::finished, this, [&](Result<void>) { ++finished; });
        job.setStep("쓰기");
        job.cancel();
        QCOMPARE(requested, 1);
        QCOMPARE(finished, 0);
        QCOMPARE(job.state(), Job::State::Running);
    }
    /** T2: finishCancelled는 Cancelled 상태와 JobError::Cancelled로 끝내요. */
    void test_T2_finish_cancelled()
    {
        Job job(OperationKind::FcFlash);
        int finished = 0;
        std::optional<Result<void>> last;
        QList<int> order;
        job.hold([&order] { order << 1; });
        connect(&job, &Job::finished, this, [&](Result<void> result) { ++finished; last = result; });
        job.setStep("쓰기");
        job.finishCancelled();
        job.finishCancelled();
        QCOMPARE(job.state(), Job::State::Cancelled);
        QCOMPARE(finished, 1);
        QVERIFY(last.has_value() && !last->has_value());
        QVERIFY(last->error() == kgc::app::job::make(JobError::Cancelled));
        QCOMPARE(order, QList<int>({1}));
    }
    /** T2: 단계·진행률·취소 가능 값이 프로퍼티와 시그널로 나와요. */
    void test_T2_step_progress_cancellable()
    {
        Job job(OperationKind::FcFlash);
        QSignalSpy step(&job, &Job::stepNameChanged);
        QSignalSpy progress(&job, &Job::progressChanged);
        QSignalSpy cancellable(&job, &Job::cancellableChanged);
        job.setStep("지우기");
        job.setProgress(0.25);
        job.setCancellable(true);
        QCOMPARE(job.stepName(), QString("지우기"));
        QCOMPARE(job.state(), Job::State::Running);
        QCOMPARE(job.progress(), 0.25);
        QVERIFY(job.cancellable());
        QCOMPARE(step.count(), 1);
        QCOMPARE(progress.count(), 1);
        QCOMPARE(cancellable.count(), 1);
    }
    /** T2: 남은 시간은 경과와 진행률로 셈하고, 1% 미만이면 모르는 값이에요. */
    void test_T2_eta_from_elapsed_and_progress()
    {
        Job job(OperationKind::FcFlash);
        job.setStep("쓰기");
        QTest::qWait(1200);
        job.setProgress(0.005);
        QCOMPARE(job.etaSeconds(), -1);
        job.setProgress(0.5);
        QVERIFY2(job.etaSeconds() >= 1 && job.etaSeconds() <= 2, qPrintable(QString::number(job.etaSeconds())));
        job.setProgress(-1);
        QCOMPARE(job.etaSeconds(), -1);
    }
};
QTEST_GUILESS_MAIN(JobTest)
#include "test_job.moc"
