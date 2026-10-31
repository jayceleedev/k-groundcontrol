// [0.2.0 T3] specs/features/0.2.0-fcflash.md
#include <kgc/app/net/Http.h>
#include <kgc/app/net/Errors.h>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPointer>
namespace kgc::app::net {
struct HttpRequest::Private {
    HttpRequest* request;
    QNetworkAccessManager* manager = nullptr;
    QPointer<QNetworkReply> reply;
    QUrl url;
    HttpOptions options;
    QString path;
    QFile file;
    QByteArray body;
    int retries = 0;
    bool download = false;
    bool completed = false;
    bool partialCreated = false;

    void detach()
    {
        if (!reply) return;
        QObject::disconnect(reply, nullptr, request, nullptr);
        reply->abort();
        reply->deleteLater();
        reply.clear();
    }
    void cleanPartial()
    {
        file.close();
        if (partialCreated) QFile::remove(file.fileName());
        partialCreated = false;
    }
    void finish(kgc::app::errors::Result<QByteArray> result)
    {
        if (completed) return;
        completed = true;
        detach();
        cleanPartial();
        QMetaObject::invokeMethod(request, [owner = QPointer<HttpRequest>(request), result = std::move(result)] {
            if (!owner) return;
            emit owner->finished(result);
            if (owner) owner->deleteLater();
        }, Qt::QueuedConnection);
    }
    bool consume()
    {
        if (!reply || completed) return false;
        const auto bytes = reply->readAll();
        if (!download) body += bytes;
        else if (file.write(bytes) != bytes.size()) {
            finish(std::unexpected(make(HttpError::WriteFailed)));
            return false;
        }
        return true;
    }
    void received()
    {
        if (!reply || completed) return;
        const auto error = reply->error();
        if (error != QNetworkReply::NoError) {
            const bool timeout = error == QNetworkReply::TimeoutError;
            detach();
            cleanPartial();
            if (retries++ < options.retries) {
                QMetaObject::invokeMethod(request, [this] { start(); }, Qt::QueuedConnection);
            } else finish(std::unexpected(make(timeout ? HttpError::TimedOut : HttpError::NetworkFailed)));
            return;
        }
        if (!consume()) return;
        if (download) {
            if (!file.flush()) { finish(std::unexpected(make(HttpError::WriteFailed))); return; }
            file.close();
            if ((QFile::exists(path) && !QFile::remove(path)) || !file.rename(path)) {
                finish(std::unexpected(make(HttpError::WriteFailed)));
                return;
            }
            partialCreated = false;
        }
        finish(std::move(body));
    }
    void start()
    {
        if (completed) return;
        if (url.scheme() != QStringLiteral("https")) {
            finish(std::unexpected(make(HttpError::NotHttps)));
            return;
        }
        body.clear();
        if (download) {
            file.setFileName(path + QStringLiteral(".part"));
            if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                finish(std::unexpected(make(HttpError::WriteFailed)));
                return;
            }
            partialCreated = true;
        }
        QNetworkRequest query(url);
        query.setTransferTimeout(options.timeout);
        query.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::UserVerifiedRedirectPolicy);
        query.setRawHeader("User-Agent", QByteArrayLiteral("K-GroundControl/" KGC_VERSION));
        for (auto it = options.headers.cbegin(); it != options.headers.cend(); ++it)
            query.setRawHeader(it.key(), it.value());
        reply = manager->get(query);
        QObject::connect(reply, &QNetworkReply::downloadProgress, request, &HttpRequest::progress);
        QObject::connect(reply, &QNetworkReply::readyRead, request, [this] { consume(); });
        QObject::connect(reply, &QNetworkReply::finished, request, [this] { received(); });
        QObject::connect(reply, &QNetworkReply::redirected, request, [this](const QUrl& target) {
            if (!redirectAllowed(reply->url(), target)) {
                finish(std::unexpected(make(HttpError::RedirectRefused)));
                return;
            }
            // 중간 응답 본문을 최종 파일이나 메모리 결과에 섞지 않아요.
            body.clear();
            if (download && (!file.resize(0) || !file.seek(0))) {
                finish(std::unexpected(make(HttpError::WriteFailed)));
                return;
            }
            reply->redirectAllowed();
        });
    }
};
HttpRequest::HttpRequest(QObject* parent) : QObject(parent), d_(std::make_unique<Private>()) { d_->request = this; }
HttpRequest::~HttpRequest() { d_->detach(); d_->cleanPartial(); }
void HttpRequest::abort() { d_->finish(std::unexpected(make(HttpError::Aborted))); }
struct Http::Private {
    QNetworkAccessManager manager;
    std::optional<bool> online;
    HttpRequest* create(Http* http, const QUrl& url, const QString& path, bool download, const HttpOptions& options)
    {
        auto* request = new HttpRequest(http);
        auto& data = *request->d_;
        data.manager = &manager;
        data.url = url;
        data.path = path;
        data.download = download;
        data.options = options;
        QObject::connect(request, &HttpRequest::finished, http, [this, http](const auto& result) {
            const bool success = result.has_value();
            if (online == success) return;
            online = success;
            emit http->onlineChanged(success);
        });
        QMetaObject::invokeMethod(request, [request] { request->d_->start(); }, Qt::QueuedConnection);
        return request;
    }
};
Http::Http(QObject* parent) : QObject(parent), d_(std::make_unique<Private>()) {}
Http::~Http() { qDeleteAll(findChildren<HttpRequest*>(QString(), Qt::FindDirectChildrenOnly)); }
HttpRequest* Http::get(const QUrl& url, const HttpOptions& options)
{
    return d_->create(this, url, {}, false, options);
}
HttpRequest* Http::download(const QUrl& url, const QString& path, const HttpOptions& options)
{
    return d_->create(this, url, path, true, options);
}
bool redirectAllowed(const QUrl& from, const QUrl& to)
{
    return from.scheme() == QStringLiteral("https") && to.scheme() == QStringLiteral("https");
}
}
