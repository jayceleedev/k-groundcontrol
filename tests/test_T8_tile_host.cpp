// [0.5.0 T8] specs/features/0.5.0-map.md
#include <kgc/app/boot/BootOptions.h>
#include <QtTest>
using kgc::app::boot::BootOptions;
class TileHostTest : public QObject {
    Q_OBJECT
private slots:
    /** T8: https://호스트[:포트] 꼴만 받아 끝 "/"를 떼고 호스트를 소문자로 다듬어요. */
    void test_T8_accepts_plain_https_host()
    {
        QCOMPARE(BootOptions::parseTileHost("https://127.0.0.1:8443/"), std::optional<QString>("https://127.0.0.1:8443"));
        QCOMPARE(BootOptions::parseTileHost("HTTPS://Example.COM:8443"), std::optional<QString>("https://example.com:8443"));
    }
    /** T8: http, 경로·질의가 있는 주소, scheme 없는 주소, 호스트 없는 주소는 값 없음이에요. */
    void test_T8_rejects_other_forms()
    {
        for (const auto* value : {"http://127.0.0.1:8443", "https://127.0.0.1:8443/tiles", "https://127.0.0.1:8443/?a=1",
                                  "127.0.0.1:8443", "https:///x"})
            QVERIFY2(!BootOptions::parseTileHost(QString::fromLatin1(value)).has_value(), value);
    }
};
QTEST_APPLESS_MAIN(TileHostTest)
#include "test_T8_tile_host.moc"
