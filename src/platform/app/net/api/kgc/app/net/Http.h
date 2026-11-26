// [0.5.0 S2 T9] specs/features/0.5.0-map.md
// [0.2.0 T3] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <kgc/app/net/Constants.h>
#include <kgc/app/net/Errors.h>
#include <QByteArray>
#include <QHash>
#include <QNetworkReply>
#include <QObject>
#include <QUrl>
#include <QVariant>
#include <chrono>
#include <memory>
namespace kgc::app::net {
/** 요청 하나의 시간·재시도·머리글을 정해요.
 * 입력: 무응답 시간 timeout, 재시도 횟수 retries, 더할 머리글 headers예요.
 * 출력: 기본값 30초, 2회, 머리글 없음이에요.
 * 실패: 없어요.
 */
struct HttpOptions {
    std::chrono::milliseconds timeout = kHttpDefaultTimeout;
    int retries = kHttpDefaultRetries;
    QHash<QByteArray, QByteArray> headers;
};
/** 진행 중인 요청 하나를 나타내요.
 * 입력: Http가 만들어 넘겨요.
 * 출력: progress와 finished 시그널이에요. finished 뒤에는 Http가 deleteLater로 지워요.
 * 실패: 실패는 finished의 오류 코드로 알려요.
 */
class HttpRequest : public QObject {
    Q_OBJECT
public:
/** 네트워크 자원을 놓아요.
 * 입력: 없어요.
 * 출력: 요청 자원을 정리해요.
 * 실패: 없어요.
 */
    ~HttpRequest() override;
/** 요청을 멈춰요.
 * 입력: 없어요.
 * 출력: finished(HttpError::Aborted)를 한 번 내요. 내려받던 .part 파일은 지워요.
 * 실패: 이미 끝났으면 아무 일도 하지 않아요.
 */
    void abort();
signals:
/** 받은 양을 알려요.
 * 입력: 받은 바이트 received와 전체 바이트 total이며, 전체를 모르면 total은 -1이에요.
 * 출력: 내려받는 동안 여러 번 나와요.
 * 실패: 없어요.
 */
    void progress(qint64 received, qint64 total);
/** 요청이 끝났음을 한 번 알려요.
 * 입력: get은 응답 본문, download는 빈 값, 실패면 오류 코드인 result예요.
 * 출력: 호출자가 시그널을 이을 수 있도록 언제나 이벤트 루프로 미뤄서 나와요.
 * 실패: 없어요.
 */
    void finished(kgc::app::errors::Result<QByteArray> result);
private:
    friend class Http;
    explicit HttpRequest(QObject* parent);
    struct Private;
    std::unique_ptr<Private> d_;
};
/** 앱의 유일한 HTTPS 통로예요.
 * 입력: 주소와 저장 경로, HttpOptions예요. GUI 스레드에서 부르고 Composition이 하나 만들어요.
 * 출력: 요청마다 HttpRequest 포인터와 onlineChanged 시그널이에요. User-Agent는 K-GroundControl/<버전>이에요.
 * 실패: https가 아닌 주소는 네트워크에 닿지 않고 finished(HttpError::NotHttps)로 끝나요.
 */
class Http : public QObject {
    Q_OBJECT
public:
/** 요청을 받을 준비를 해요.
 * 입력: 수명을 함께할 parent예요.
 * 출력: 아직 요청이 없는 객체예요.
 * 실패: 없어요.
 */
    explicit Http(QObject* parent = nullptr);
/** 네트워크 자원을 놓아요.
 * 입력: 없어요.
 * 출력: 남은 요청을 정리해요.
 * 실패: 없어요.
 */
    ~Http() override;
/** 응답을 메모리로 받아요.
 * 입력: 주소 url과 options예요.
 * 출력: Http가 소유하는 HttpRequest 포인터이며, 언제나 널이 아니에요.
 * 실패: finished의 오류 코드로 알려요. https가 아니면 NotHttps예요.
 */
    HttpRequest* get(const QUrl& url, const HttpOptions& options = {});
/** 응답을 파일로 내려받아요.
 * 입력: 주소 url, 최종 파일 path, options예요.
 * 출력: Http가 소유하는 HttpRequest 포인터이며, path + ".part"에 쓰고 끝나면 이름을 바꿔요.
 * 실패: finished의 오류 코드로 알려요. https가 아니면 NotHttps이고 파일을 만들지 않아요.
 */
    HttpRequest* download(const QUrl& url, const QString& path, const HttpOptions& options = {});
signals:
/** 서버에 닿는지가 바뀌었음을 알려요.
 * 입력: 마지막 요청이 성공했거나 서버가 HTTP 상태로 거절했으면(HttpStatus) 참인 online이에요.
 * 출력: 값이 바뀔 때 나와요. 취소(Aborted)로 끝난 요청은 판정을 바꾸지 않아요.
 * 실패: 없어요.
 */
    void onlineChanged(bool online);
private:
    struct Private;
    std::unique_ptr<Private> d_;
};
/** 리다이렉트를 따라가도 되는지 판정해요.
 * 입력: 지금 주소 from과 옮겨 갈 주소 to예요.
 * 출력: 둘 다 https이면 참이에요.
 * 실패: 없어요.
 */
bool redirectAllowed(const QUrl& from, const QUrl& to);
/** 마지막 실패의 HTTP 오류를 정해요.
 * 입력: 응답의 네트워크 오류 error와 응답 상태 속성 httpStatus(없으면 빈 QVariant)예요.
 * 출력: httpStatus가 유효한 수이고 kHttpErrorStatusMin(400) 이상이면 HttpStatus, 아니고 error가 TimeoutError면 TimedOut,
 *       그 밖은 NetworkFailed예요. 200 머리글 뒤 본문 중에 끊긴 것은 상태 오류가 아니에요.
 * 실패: 없어요.
 */
HttpError failureOf(QNetworkReply::NetworkError error, const QVariant& httpStatus);
/** 시험 서버 인증서를 이번 실행의 모든 HTTPS 요청이 믿게 해요.
 * 입력: PEM 인증서 파일 경로 pemPath예요. 첫 HTTPS 요청 전에 한 번만 불러요(main이 --tile-ca로 불러요).
 * 출력: 기본 SSL 설정에 그 인증서를 더해요. 시스템 루트 인증서도 계속 믿어요. 그 실행의 모든 HTTPS 요청에 적용돼요.
 * 실패: 파일을 읽지 못하거나 인증서가 없으면 BadCertificate이고 설정은 그대로예요.
 */
kgc::app::errors::Result<void> trustCertificateFile(const QString& pemPath);
/** gzip 묶음을 풀어요.
 * 입력: gzip 바이트 bytes예요.
 * 출력: 풀린 바이트예요.
 * 실패: 깨졌거나 잘렸으면 HttpError::BadGzip이에요.
 */
kgc::app::errors::Result<QByteArray> gunzip(const QByteArray& bytes);
}
