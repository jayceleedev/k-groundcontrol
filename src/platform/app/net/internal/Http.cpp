// [0.2.0 T3] specs/features/0.2.0-fcflash.md
#include <kgc/app/net/Http.h>
#include <kgc/app/net/Errors.h>
namespace kgc::app::net {
struct HttpRequest::Private {};
HttpRequest::HttpRequest(QObject* parent) : QObject(parent), d_(std::make_unique<Private>()) {}
HttpRequest::~HttpRequest() = default;
void HttpRequest::abort()
{
    // WORKER-FILL: spec §5.4 — 응답을 멈추고 .part를 지운 뒤 finished(Aborted)를 한 번 내요.
}
struct Http::Private {};
Http::Http(QObject* parent) : QObject(parent), d_(std::make_unique<Private>()) {}
Http::~Http() = default;
HttpRequest* Http::get(const QUrl& url, const HttpOptions& options)
{
    // WORKER-FILL: spec §5.4 — https 확인, QNetworkAccessManager로 요청, 무응답 시간과 재시도, redirectAllowed, User-Agent.
    Q_UNUSED(url);
    Q_UNUSED(options);
    return new HttpRequest(this);
}
HttpRequest* Http::download(const QUrl& url, const QString& path, const HttpOptions& options)
{
    // WORKER-FILL: spec §5.4 — get과 같되 path + ".part"에 쓰고 끝나면 이름을 바꿔요. 결과는 빈 값이에요.
    Q_UNUSED(url);
    Q_UNUSED(path);
    Q_UNUSED(options);
    return new HttpRequest(this);
}
bool redirectAllowed(const QUrl& from, const QUrl& to)
{
    // WORKER-FILL: spec §5.4 — 둘 다 https일 때만 참이에요.
    Q_UNUSED(from);
    Q_UNUSED(to);
    return true;
}
}
