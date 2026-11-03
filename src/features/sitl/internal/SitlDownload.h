// [0.4.0 5.4 6.4] specs/features/0.4.0-sitl.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <kgc/sitl/SitlTypes.h>
#include <QList>
#include <QObject>
#include <QPointer>
namespace kgc::app::job { class Job; }
namespace kgc::app::net { class Http; class HttpRequest; }
namespace kgc::sitl {
/** SITL 파일을 차례로 내려받아 캐시 폴더와 바꾸는 Job 하나를 돌려요.
 * 입력: Http, 받을 파일 목록, 캐시 바탕 폴더와 폴더 이름, 실행 권한을 줄지 여부예요.
 * 출력: job()으로 진행을 보이고, 끝나면 finished를 한 번 내요.
 * 실패: 받기 실패는 DownloadFailed, 취소는 JobError::Cancelled로 끝나요. 어느 쪽이든 .part 폴더를 지워요.
 */
class SitlDownload : public QObject {
    Q_OBJECT
public:
/** 아직 시작하지 않은 내려받기를 만들어요.
 * 입력: Http, 받을 파일 files(첫 항목이 실행 파일), 캐시 바탕 폴더 cacheRoot, 폴더 이름 folderName,
 *       실행 권한을 줄지 executable, parent예요.
 * 출력: Pending 상태의 Job(OperationKind::Read)을 가진 객체예요.
 * 실패: 없어요.
 */
    SitlDownload(kgc::app::net::Http& http, QList<SitlFile> files, QString cacheRoot, QString folderName,
                 bool executable, QObject* parent);
    /** 진행 중인 요청을 멈추고 .part 폴더를 지워요. finished는 내지 않아요. */
    ~SitlDownload() override;
    /** 입력: 없어요. 출력: 이 내려받기의 Job이에요. 소유권은 넘기지 않아요. 실패: 없어요. */
    kgc::app::job::Job* job() const;
/** 첫 파일부터 받기 시작해요.
 * 입력: 없어요.
 * 출력: Job이 Running이 되고 파일마다 Http::download를 불러요.
 * 실패: .part 폴더를 만들 수 없으면 DownloadFailed로 끝나요.
 */
    void start();
signals:
    /** 입력: 없어요. 출력: 성공이면 값 없는 결과, 실패·취소면 오류 코드인 result예요. 한 번만 나와요. 실패: 없어요. */
    void finished(kgc::app::errors::Result<void> result);
private:
    void next();
    void fileFinished(const kgc::app::errors::Result<QByteArray>& result);
    void complete();
    void cancel();
    void finishWith(const QString& reason);
    kgc::app::net::Http& http_;
    QList<SitlFile> files_;
    QString cacheRoot_;
    QString folderName_;
    QString partDir_;
    bool executable_;
    int index_ = 0;
    bool done_ = false;
    kgc::app::job::Job* job_;
    QPointer<kgc::app::net::HttpRequest> request_;
};
}
