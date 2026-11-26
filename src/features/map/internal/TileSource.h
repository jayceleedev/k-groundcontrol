// [0.5.0 5.6.6] specs/features/0.5.0-map.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <kgc/app/net/Http.h>
#include <kgc/app/scheduler/Timeout.h>
#include <kgc/map/Settings.h>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QString>
#include <QUrl>
#include <functional>
namespace kgc::map {
class TileStore;
// fetch가 돌려주는 대기표예요. 0은 "없음"이에요.
using TileTicket = quint64;
/** 모든 지도와 영역 받기가 같이 쓰는 타일 공급이에요(GUI 스레드).
 * 입력: Http, 타일 저장소, 시험 서버 주소(비면 Google)예요. fetch로 타일을 청해요.
 * 출력: 같은 타일 요청은 하나로 묶고, 저장소에 있으면 저장소에서, 없으면 서버에서 받아 저장한 뒤 콜백으로 줘요.
 *       서버에 다시 닿으면 onlineAgain을 내요.
 * 실패: 콜백에 Http 오류나 MapError::TileFailed(그림이 아닌 답)를 넘겨요. 키마다 한 번 "Tile failed" 로그를 남겨요.
 */
class TileSource : public QObject {
    Q_OBJECT
public:
/** 저장소 답을 받을 준비를 해요.
 * 입력: http, 저장소 store(저장소 스레드에 있음), 시험 서버 host, 부모 parent예요.
 * 출력: 언어가 빈 공급이에요. 저장소 시그널을 여기서 이어요(저장소 스레드 시작 전에 만들어야 해요).
 * 실패: 없어요.
 */
    TileSource(kgc::app::net::Http& http, TileStore& store, QString host, QObject* parent);
/** 지명 언어를 읽어요.
 * 입력: 없어요.
 * 출력: "ko" 또는 "en"이에요.
 * 실패: 없어요.
 */
    QString language() const;
/** 지명 언어를 바꿔요.
 * 입력: 새 언어 language예요.
 * 출력: 같은 값이면 아무것도 하지 않고, 다르면 languageChanged를 내요.
 * 실패: 없어요.
 */
    void setLanguage(const QString& language);
/** 타일 한 장을 청해요.
 * 입력: 지도 종류 type, 언어 language, 확대 z와 타일 x·y, 미리 받기(고정) 여부 pin, 끝나면 부를 done이에요.
 * 출력: 대기표예요. done은 언제나 나중에(이벤트 루프를 거쳐) 한 번 불려요. pin이면 이미 저장된 타일은 고정만 하고 빈 바이트로 성공해요.
 *       같은 타일을 기다리는 요청이 있으면 그 요청에 끼워요(서버 요청을 새로 내지 않아요).
 * 실패: done에 오류를 넘겨요.
 */
    TileTicket fetch(settings::MapType type, const QString& language, int z, int x, int y, bool pin,
                     std::function<void(kgc::app::errors::Result<QByteArray>)> done);
/** 대기표를 거둬요.
 * 입력: fetch가 준 ticket이에요.
 * 출력: 그 done은 불리지 않아요. 그 타일을 기다리는 사람이 없으면 대기를 지우고 서버 요청을 멈춰요.
 * 실패: 이미 끝났거나 모르는 표면 아무 일도 하지 않아요.
 */
    void cancel(TileTicket ticket);
signals:
/** 지명 언어가 바뀌었음을 알려요. 입력: 없어요. 출력: 없어요. 실패: 없어요. */
    void languageChanged();
/** 닿지 않던 서버에 다시 닿았음을 알려요. 입력: 없어요. 출력: 없어요. 실패: 없어요. */
    void onlineAgain();
private:
    using Done = std::function<void(kgc::app::errors::Result<QByteArray>)>;
    struct Waiter { TileTicket ticket; bool pin; Done done; };
    enum class Stage { Pin, Lookup, Http };
    struct Pending {
        Stage stage;
        quint64 requestId = 0;
        QPointer<kgc::app::net::HttpRequest> request;
        int retries = 0;
        QList<Waiter> waiters;
        settings::MapType type;
        QString language;
        int z = 0;
        int x = 0;
        int y = 0;
    };
    QHash<QString, Pending>::iterator storeEntry(quint64 requestId);
    void storeFound(quint64 requestId, const QByteArray& bytes);
    void storeMissing(quint64 requestId);
    void storePinned(quint64 requestId, bool pinned);
    void startHttp(const QString& key);
    void httpFinished(const QString& key, const kgc::app::net::HttpRequest* request,
                      const kgc::app::errors::Result<QByteArray>& result);
    void deliverFailure(const QString& key, const QList<Waiter>& waiters, kgc::app::errors::ErrorCode error);
    void reach(bool reached, const QUrl& url);
    void probe();
    kgc::app::net::Http& http_;
    TileStore& store_;
    QString host_;
    QString language_;
    QHash<QString, Pending> pending_;
    quint64 nextRequestId_ = 0;
    TileTicket nextTicket_ = 0;
    QSet<QString> failedKeys_;
    bool online_ = true;
    QUrl probeUrl_;
    kgc::app::scheduler::Timeout probeTimeout_;
};
/** 오류 코드를 로그와 Qt 오류 글자에 쓰는 짧은 이름으로 바꿔요.
 * 입력: 오류 코드 error예요.
 * 출력: "<모듈 번호>:<오류 번호>" 꼴(영어만, 예: "21:1")이에요.
 * 실패: 없어요.
 */
QString tileErrorName(kgc::app::errors::ErrorCode error);
}
