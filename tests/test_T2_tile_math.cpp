// [0.5.0 T2] specs/features/0.5.0-map.md
#include "TileMath.h"
#include <QtTest>
using kgc::map::metersPerPixel;
using kgc::map::tileCount;
using kgc::map::tileOf;
using kgc::map::tileRange;
namespace {
// 북·남·서·동 경계로 영역을 만들어요(왼쪽 위 = 북서, 오른쪽 아래 = 남동).
QGeoRectangle area(double north, double south, double west, double east)
{
    return QGeoRectangle(QGeoCoordinate(north, west), QGeoCoordinate(south, east));
}
// 타일 묶음을 비교하기 쉬운 목록으로 바꿔요.
QList<int> parts(const kgc::map::TileRange& range)
{
    return {range.x, range.y, range.columns, range.rows};
}
}
class TileMathTest : public QObject {
    Q_OBJECT
private slots:
    /** T2: 웹 메르카토르 타일 번호이고, 극과 날짜 변경선 끝은 0 ~ 2^z − 1로 잘려요. */
    void test_T2_tile_of()
    {
        const auto check = [](double lat, double lon, int z, int x, int y) {
            const auto tile = tileOf(lat, lon, z);
            QCOMPARE(tile.x, x);
            QCOMPARE(tile.y, y);
        };
        check(35.1587, 129.1604, 15, 28140, 12961);
        check(35.1587, 129.1604, 7, 109, 50);
        check(37.5665, 126.9780, 19, 447069, 203031);
        check(-33.8568, 151.2153, 10, 942, 614);
        check(89, -180, 3, 0, 0);
        check(-89, 180, 3, 7, 7);
    }
    /** T2: 영역의 왼쪽 위 타일과 열·행 수이고, 날짜 변경선을 넘으면 열이 오른쪽 끝에서 왼쪽 끝으로 이어져요. */
    void test_T2_tile_range()
    {
        QCOMPARE(parts(tileRange(area(35.17, 35.15, 129.15, 129.17), 15)), QList<int>({28139, 12960, 3, 3}));
        QCOMPARE(parts(tileRange(area(10, 1, 170, -170), 3)), QList<int>({7, 3, 2, 1}));
        QCOMPARE(parts(tileRange(area(10, 1, 170, -170), 10)), QList<int>({995, 483, 58, 27}));
        QCOMPARE(parts(tileRange(area(10, 1, 10, 5), 3)), QList<int>({4, 3, 8, 1}));
    }
    /** T2: 확대 범위의 타일 수는 확대마다 열 × 행의 합이에요(한국 영역 15~17 = 9 + 24 + 80). */
    void test_T2_tile_count()
    {
        QCOMPARE(tileCount(area(35.17, 35.15, 129.15, 129.17), 15, 15), qint64(9));
        QCOMPARE(tileCount(area(35.17, 35.15, 129.15, 129.17), 16, 16), qint64(24));
        QCOMPARE(tileCount(area(35.17, 35.15, 129.15, 129.17), 17, 17), qint64(80));
        QCOMPARE(tileCount(area(35.17, 35.15, 129.15, 129.17), 15, 17), qint64(113));
        QCOMPARE(tileCount(area(10, 1, 170, -170), 3, 5), qint64(6));
    }
    /** T2: 해운대 위도 확대 15에서 1 px은 약 3.9057 m예요. */
    void test_T2_meters_per_pixel()
    {
        QVERIFY(qAbs(metersPerPixel(35.1587, 15) - 3.9057) <= 0.001);
    }
};
QTEST_APPLESS_MAIN(TileMathTest)
#include "test_T2_tile_math.moc"
