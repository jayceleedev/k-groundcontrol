// [0.4.0 5.4] specs/features/0.4.0-sitl.md
#pragma once
#include <QObject>
#include <QStringList>
class QProcess;
namespace kgc::sitl {
/** 시뮬레이터 프로세스 하나를 띄우고 멈춰요. 앱에서 QProcess를 쥐는 유일한 곳이에요.
 * 입력: 실행 파일, 인자, 작업 폴더와 멈춤 요청이에요.
 * 출력: started·failedToStart·exited·line 시그널이에요. 표준 출력과 오류를 합쳐 줄 단위로 내요.
 * 실패: 실행하지 못하면 failedToStart로 알려요. 자식 환경 변수는 건드리지 않아요.
 */
class Simulator : public QObject {
    Q_OBJECT
public:
    /** 프로세스 없이 만들어요. 입력: 선택적인 parent예요. 출력: 멈춘 상태예요. 실패: 없어요. */
    explicit Simulator(QObject* parent = nullptr);
    /** 남은 프로세스를 stop과 같은 방법으로 멈춘 뒤 지워져요. */
    ~Simulator() override;
/** 프로세스를 띄워요.
 * 입력: 실행 파일 절대 경로 program, 인자 arguments, 작업 폴더 workingDirectory예요.
 * 출력: 뜨면 started(pid), 못 뜨면 failedToStart(까닭)를 내요.
 * 실패: 이미 돌고 있으면 프로그래밍 오류예요.
 */
    void start(const QString& program, const QStringList& arguments, const QString& workingDirectory);
/** 프로세스를 강제로 끝내고 kSitlStopWait까지 기다려요.
 * 입력: 없어요.
 * 출력: 끝나면 exited(…, requested = 참)를 내요.
 * 실패: 돌고 있지 않으면 아무 일도 하지 않아요.
 */
    void stop();
    /** 입력: 없어요. 출력: 프로세스가 떠 있거나 뜨는 중이면 참이에요. 실패: 없어요. */
    bool running() const;
signals:
    /** 입력: 없어요. 출력: 프로세스가 떴고 그 번호 pid예요. 실패: 없어요. */
    void started(qint64 pid);
    /** 입력: 없어요. 출력: 실행하지 못한 까닭(Qt의 영어 문구) reason이에요. 실패: 없어요. */
    void failedToStart(QString reason);
    /** 입력: 없어요. 출력: 종료 코드 exitCode, 강제 종료 여부 crashed, stop으로 멈췄는지 requested예요. 실패: 없어요. */
    void exited(int exitCode, bool crashed, bool requested);
    /** 입력: 없어요. 출력: 출력 한 줄 text이며 끝의 줄바꿈은 빼요. 실패: 없어요. */
    void line(QString text);
private:
    void readLines();
    QProcess* process_ = nullptr;
    QByteArray pending_;
    bool requested_ = false;
};
}
