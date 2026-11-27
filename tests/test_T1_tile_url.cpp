// [0.5.1 T2] specs/features/0.5.1-map-sharp.md
// [0.5.0 T1] specs/features/0.5.0-map.md
#include "TileImage.h"
#include "TileUrl.h"
#include <QtTest>
using kgc::map::settings::MapType;
using kgc::map::tileImageFormat;
using kgc::map::tileKey;
using kgc::map::tileUrl;
class TileUrlTest : public QObject {
    Q_OBJECT
private slots:
    /** T1: Google 주소는 서버 번호 (x + y) mod 4와 lyrs·hl·x·y·z를 붙이고, 위성도 hl을 붙여요. */
    void test_T1_google_urls()
    {
        QCOMPARE(tileUrl(MapType::Satellite, QStringLiteral("ko"), 1, 15, 28140, 12961, {}),
                 QStringLiteral("https://mt1.google.com/vt/lyrs=s&hl=ko&x=28140&y=12961&z=15"));
        QCOMPARE(tileUrl(MapType::Hybrid, QStringLiteral("en"), 1, 15, 28140, 12961, {}),
                 QStringLiteral("https://mt1.google.com/vt/lyrs=y&hl=en&x=28140&y=12961&z=15"));
        QCOMPARE(tileUrl(MapType::Road, QStringLiteral("ko"), 1, 7, 109, 50, {}),
                 QStringLiteral("https://mt3.google.com/vt/lyrs=m&hl=ko&x=109&y=50&z=7"));
    }
    /** T1: 시험 서버 주소를 주면 Google 앞부분 대신 그 주소를 그대로 써요. */
    void test_T1_test_host_url()
    {
        QCOMPARE(tileUrl(MapType::Satellite, QStringLiteral("ko"), 1, 15, 28140, 12961, QStringLiteral("https://127.0.0.1:8443")),
                 QStringLiteral("https://127.0.0.1:8443/vt/lyrs=s&hl=ko&x=28140&y=12961&z=15"));
    }
    /** T1: 저장소 열쇠는 위성만 언어를 빼고, 하이브리드·일반은 언어를 넣어요. */
    void test_T1_tile_keys()
    {
        QCOMPARE(tileKey(MapType::Satellite, QStringLiteral("ko"), 1, 15, 28140, 12961), QStringLiteral("s/15/28140/12961"));
        QCOMPARE(tileKey(MapType::Hybrid, QStringLiteral("ko"), 1, 15, 28140, 12961), QStringLiteral("y/ko/15/28140/12961"));
        QCOMPARE(tileKey(MapType::Road, QStringLiteral("en"), 1, 7, 109, 50), QStringLiteral("m/en/7/109/50"));
    }
    /** T2: 2배 타일 주소만 끝에 &scale=2를 붙여요(Google과 시험 서버 모두). */
    void test_T2_double_ratio_urls()
    {
        QCOMPARE(tileUrl(MapType::Satellite, QStringLiteral("ko"), 2, 15, 28140, 12961, {}),
                 QStringLiteral("https://mt1.google.com/vt/lyrs=s&hl=ko&x=28140&y=12961&z=15&scale=2"));
        QCOMPARE(tileUrl(MapType::Road, QStringLiteral("ko"), 2, 7, 109, 50, QStringLiteral("https://127.0.0.1:8443")),
                 QStringLiteral("https://127.0.0.1:8443/vt/lyrs=m&hl=ko&x=109&y=50&z=7&scale=2"));
    }
    /** T2: 2배 타일 열쇠만 끝에 @2를 붙이고, 1배 열쇠는 0.5.0과 같아요. */
    void test_T2_double_ratio_keys()
    {
        QCOMPARE(tileKey(MapType::Satellite, QStringLiteral("ko"), 2, 15, 28140, 12961), QStringLiteral("s/15/28140/12961@2"));
        QCOMPARE(tileKey(MapType::Hybrid, QStringLiteral("en"), 2, 15, 28140, 12961), QStringLiteral("y/en/15/28140/12961@2"));
        QCOMPARE(tileKey(MapType::Road, QStringLiteral("ko"), 1, 7, 109, 50), QStringLiteral("m/ko/7/109/50"));
    }
    /** T1: 앞 바이트가 JPEG·PNG 머리이면 그 형식, 아니면(HTML, 잘린 머리, 빈 값) 빈 문자열이에요. */
    void test_T1_image_format()
    {
        QCOMPARE(tileImageFormat(QByteArray::fromHex("ffd8ffe000")), QStringLiteral("jpeg"));
        QCOMPARE(tileImageFormat(QByteArray::fromHex("89504e470d0a1a0a")), QStringLiteral("png"));
        QCOMPARE(tileImageFormat(QByteArrayLiteral("<html>")), QString());
        QCOMPARE(tileImageFormat(QByteArray::fromHex("ffd8")), QString());
        QCOMPARE(tileImageFormat(QByteArray()), QString());
    }
};
QTEST_APPLESS_MAIN(TileUrlTest)
#include "test_T1_tile_url.moc"
