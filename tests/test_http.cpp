// [0.2.0 T3] specs/features/0.2.0-fcflash.md
#include <kgc/app/net/Http.h>
#include <kgc/app/net/Errors.h>
#include <QTemporaryDir>
#include <QtTest>
using kgc::app::errors::Result;
using kgc::app::net::Http;
using kgc::app::net::HttpError;
using kgc::app::net::HttpOptions;
using kgc::app::net::HttpRequest;
using kgc::app::net::gunzip;
using kgc::app::net::make;
using kgc::app::net::redirectAllowed;
namespace {
const QByteArray kHelloGzip = QByteArray::fromHex("1f8b08000000000002ffcb48cdc9c95748afca2ce00200397c63560b000000");
const QByteArray kManyLettersGzip = QByteArray::fromHex(
    "1f8b08000000000002ffedc13101000000c2a06ceb5fca0cfe4001000000000000000000000000000000000000000000"
    "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    "00000000000000000000000000000000000000000000000000000000af01d16aebac400d0300");
}
class HttpTest : public QObject {
    Q_OBJECT
private slots:
    /** T3: 리다이렉트는 https에서 https로만 따라가요. */
    void test_T3_redirect_only_between_https()
    {
        const QUrl secure("https://example.com/a");
        QVERIFY(redirectAllowed(secure, QUrl("https://release-assets.githubusercontent.com/b")));
        QVERIFY(!redirectAllowed(secure, QUrl("http://example.com/a")));
        QVERIFY(!redirectAllowed(secure, QUrl("ftp://example.com/a")));
        QVERIFY(!redirectAllowed(QUrl("http://example.com/a"), secure));
    }
    /** T3: https가 아닌 주소의 get은 네트워크에 닿지 않고 NotHttps로 끝나요. */
    void test_T3_get_rejects_http()
    {
        Http http;
        auto* request = http.get(QUrl("http://example.com/manifest.json"));
        QVERIFY(request);
        std::optional<Result<QByteArray>> result;
        connect(request, &HttpRequest::finished, this, [&](Result<QByteArray> value) { result = value; });
        QTRY_VERIFY_WITH_TIMEOUT(result.has_value(), 1000);
        QVERIFY(!result->has_value());
        QVERIFY(result->error() == make(HttpError::NotHttps));
    }
    /** T3: https가 아닌 주소의 download도 NotHttps로 끝나고 파일을 만들지 않아요. */
    void test_T3_download_rejects_http()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const auto path = dir.filePath("firmware.px4");
        Http http;
        auto* request = http.download(QUrl("http://example.com/firmware.px4"), path);
        QVERIFY(request);
        std::optional<Result<QByteArray>> result;
        connect(request, &HttpRequest::finished, this, [&](Result<QByteArray> value) { result = value; });
        QTRY_VERIFY_WITH_TIMEOUT(result.has_value(), 1000);
        QVERIFY(!result->has_value());
        QVERIFY(result->error() == make(HttpError::NotHttps));
        QVERIFY(!QFile::exists(path));
        QVERIFY(!QFile::exists(path + ".part"));
    }
    /** T3: HttpOptions의 기본값은 무응답 30초, 재시도 2회예요. */
    void test_T3_default_options()
    {
        const HttpOptions options;
        QCOMPARE(options.timeout, std::chrono::milliseconds(30000));
        QCOMPARE(options.retries, 2);
        QVERIFY(options.headers.isEmpty());
    }
    /** T3: gzip 벡터를 풀어요. */
    void test_T3_gunzip_vector()
    {
        const auto result = gunzip(kHelloGzip);
        QVERIFY(result.has_value());
        QCOMPARE(*result, QByteArray("hello gzip\n"));
    }
    /** T3: 입력보다 훨씬 크게 풀리는 벡터도 끝까지 풀어요. */
    void test_T3_gunzip_large_output()
    {
        const auto result = gunzip(kManyLettersGzip);
        QVERIFY(result.has_value());
        QCOMPARE(*result, QByteArray(200000, 'A'));
    }
    /** T3: 깨졌거나 잘린 입력은 BadGzip이에요. */
    void test_T3_gunzip_rejects_bad_input()
    {
        for (const auto& bad : {QByteArray("not gzip"), kHelloGzip.first(12), QByteArray()}) {
            const auto result = gunzip(bad);
            QVERIFY(!result.has_value());
            QVERIFY(result.error() == make(HttpError::BadGzip));
        }
    }
};
QTEST_GUILESS_MAIN(HttpTest)
#include "test_http.moc"
