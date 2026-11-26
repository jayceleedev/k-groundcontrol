// [0.5.0 T3] specs/features/0.5.0-map.md
#include "ScaleBar.h"
#include "SizeText.h"
#include <QtTest>
using kgc::map::scaleBar;
using kgc::map::scaleLabel;
using kgc::map::sizeText;
class ScaleSizeTest : public QObject {
    Q_OBJECT
private slots:
    /** T3: 축척 막대는 {1, 2, 5} × 10^k m 가운데 폭 안에 드는 가장 긴 것이고 px은 반올림이에요. */
    void test_T3_scale_bar()
    {
        const auto check = [](double metersPerPixel, int maxPixels, double meters, int pixels) {
            const auto length = scaleBar(metersPerPixel, maxPixels);
            QCOMPARE(length.meters, meters);
            QCOMPARE(length.pixels, pixels);
        };
        check(3.9057, 120, 200, 51);
        check(1.0, 120, 100, 100);
        check(50, 120, 5000, 100);
    }
    /** T3: 1000 m 미만은 m, 이상은 km이고 정수가 아니면 소수 한 자리예요. */
    void test_T3_scale_label()
    {
        QCOMPARE(scaleLabel(200), QStringLiteral("200 m"));
        QCOMPARE(scaleLabel(5000), QStringLiteral("5 km"));
        QCOMPARE(scaleLabel(1500), QStringLiteral("1.5 km"));
    }
    /** T3: 1024 단위 KB·MB·GB를 반올림 전에 고르고, 소수 한 자리에서 끝의 .0을 빼요. */
    void test_T3_size_text()
    {
        QCOMPARE(sizeText(0), QStringLiteral("0 KB"));
        QCOMPARE(sizeText(512), QStringLiteral("0.5 KB"));
        QCOMPARE(sizeText(15360), QStringLiteral("15 KB"));
        QCOMPARE(sizeText(29076480), QStringLiteral("27.7 MB"));
        QCOMPARE(sizeText(859832320), QStringLiteral("820 MB"));
        QCOMPARE(sizeText(1288490189), QStringLiteral("1.2 GB"));
        QCOMPARE(sizeText(2147483648LL), QStringLiteral("2 GB"));
        QCOMPARE(sizeText(1073741823), QStringLiteral("1024 MB"));
    }
};
QTEST_APPLESS_MAIN(ScaleSizeTest)
#include "test_T3_scale_size.moc"
