// [0.5.0 T4] specs/features/0.5.0-map.md
#include "Trail.h"
#include <QtTest>
#include <limits>
using kgc::map::appendTrailPoint;
using kgc::map::isMapPosition;
namespace {
const QGeoCoordinate kStart(35.1587, 129.1604);
// 점 from에서 북쪽으로 meters만큼 간 곳이에요.
QGeoCoordinate north(const QGeoCoordinate& from, double meters)
{
    return from.atDistanceAndAzimuth(meters, 0);
}
// 북쪽으로 2 m 간격인 점 count개의 목록이에요.
QList<QGeoCoordinate> line(int count)
{
    QList<QGeoCoordinate> points{kStart};
    while (points.size() < count) points.append(north(points.constLast(), 2));
    return points;
}
}
class TrailTest : public QObject {
    Q_OBJECT
private slots:
    /** T4: 둘 다 유한하고 둘 다 0은 아닌 위치만 지도 위치예요. */
    void test_T4_map_position()
    {
        const double nan = std::numeric_limits<double>::quiet_NaN();
        QVERIFY(isMapPosition(35.1, 129.1));
        QVERIFY(!isMapPosition(0, 0));
        QVERIFY(isMapPosition(0, 129.1));
        QVERIFY(isMapPosition(35.1, 0));
        QVERIFY(!isMapPosition(nan, 129.1));
    }
    /** T4: 마지막 점에서 1 m보다 많이 움직였을 때만 더하고, 위치 없음·(0, 0)은 거르고 개수를 그대로 둬요. */
    void test_T4_append_spacing()
    {
        QList<QGeoCoordinate> points;
        QVERIFY(appendTrailPoint(points, kStart, 1.0, 20000));
        QCOMPARE(points.size(), 1);
        QVERIFY(!appendTrailPoint(points, north(kStart, 0.5), 1.0, 20000));
        QCOMPARE(points.size(), 1);
        QVERIFY(appendTrailPoint(points, north(kStart, 1.5), 1.0, 20000));
        QCOMPARE(points.size(), 2);
        const double nan = std::numeric_limits<double>::quiet_NaN();
        QVERIFY(!appendTrailPoint(points, QGeoCoordinate(nan, 129.1604), 1.0, 20000));
        QVERIFY(!appendTrailPoint(points, QGeoCoordinate(0, 0), 1.0, 20000));
        QCOMPARE(points.size(), 2);
    }
    /** T4: 최대 점 수를 넘으면 넘는 만큼 가장 오래된 점부터 지워요. */
    void test_T4_append_limit()
    {
        auto three = line(3);
        const auto second = three.at(1);
        QVERIFY(appendTrailPoint(three, north(three.constLast(), 2), 1.0, 3));
        QCOMPARE(three.size(), 3);
        QCOMPARE(three.constFirst(), second);
        auto five = line(5);
        const auto fourth = five.at(3);
        QVERIFY(appendTrailPoint(five, north(five.constLast(), 2), 1.0, 3));
        QCOMPARE(five.size(), 3);
        QCOMPARE(five.constFirst(), fourth);
    }
};
QTEST_APPLESS_MAIN(TrailTest)
#include "test_T4_trail.moc"
