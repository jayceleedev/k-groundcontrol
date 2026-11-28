// [0.6.0 T3] specs/features/0.6.0-fly.md
#include <kgc/app/geo/Geodesy.h>
#include <kgc/app/geo/Altitude.h>
#include <QtTest>
#include <cmath>
#include <limits>
using kgc::app::geo::AltitudeFrame;
using kgc::app::geo::altitudeIn;
using kgc::app::geo::distanceMeters;
using kgc::app::geo::heightIn;
using kgc::app::geo::isKnownPosition;
namespace {
constexpr double kNan = std::numeric_limits<double>::quiet_NaN();
bool near(std::optional<double> value, double expected, double tolerance)
{
    return value.has_value() && std::abs(*value - expected) <= tolerance;
}
}
class GeoTest : public QObject {
    Q_OBJECT
private slots:
    /** T3: 하버사인 거리는 Qt와 같은 반지름으로 재고, 값 하나라도 NaN이면 NaN이에요. */
    void test_T3_distance()
    {
        QCOMPARE(distanceMeters(35.1587, 129.1604, 35.1587, 129.1604), 0.0);
        QVERIFY(near(distanceMeters(35.1587, 129.1604, 35.1677, 129.1604), 1000.755, 0.01));
        QVERIFY(near(distanceMeters(35.1587, 129.1604, 35.1587, 129.1714), 999.995, 0.01));
        QVERIFY(near(distanceMeters(37.5665, 126.9780, 35.1796, 129.0756), 325111.626, 0.1));
        QVERIFY(near(distanceMeters(35.1587, 129.1604, 35.1605, 129.1622), 258.527, 0.01));
        QVERIFY(std::isnan(distanceMeters(kNan, 129.1604, 35.1605, 129.1622)));
    }
    /** T3: 둘 다 유한하고 (0, 0)이 아니면 아는 위치예요. */
    void test_T3_known_position()
    {
        QVERIFY(isKnownPosition(35.1, 129.1));
        QVERIFY(!isKnownPosition(0, 0));
        QVERIFY(isKnownPosition(0, 129.1));
        QVERIFY(!isKnownPosition(kNan, 129.1));
    }
    /** T3: 홈 기준 고도를 홈 기준·해수면 틀로 바꾸고, 지금 자리 위 틀이나 모르는 값은 값이 없어요. */
    void test_T3_altitude_in()
    {
        QVERIFY(near(altitudeIn(AltitudeFrame::RelativeHome, 30, kNan), 30, 1e-9));
        QVERIFY(near(altitudeIn(AltitudeFrame::Amsl, 30, 3.07), 33.07, 1e-9));
        QVERIFY(!altitudeIn(AltitudeFrame::Amsl, 30, kNan).has_value());
        QVERIFY(!altitudeIn(AltitudeFrame::RelativeHome, kNan, 3).has_value());
        QVERIFY(!altitudeIn(AltitudeFrame::AboveCurrent, 30, 3).has_value());
    }
    /** T3: 지금 자리 위 높이를 세 틀로 바꾸고, 홈보다 낮은 땅은 0으로 봐요. */
    void test_T3_height_in()
    {
        QVERIFY(near(heightIn(AltitudeFrame::AboveCurrent, 40, kNan, kNan), 40, 1e-9));
        QVERIFY(near(heightIn(AltitudeFrame::RelativeHome, 20, 0.3, kNan), 20.3, 1e-9));
        QVERIFY(near(heightIn(AltitudeFrame::RelativeHome, 20, -2.4, kNan), 20, 1e-9));
        QVERIFY(!heightIn(AltitudeFrame::RelativeHome, 20, kNan, 3).has_value());
        QVERIFY(near(heightIn(AltitudeFrame::Amsl, 10, kNan, 3.07), 13.07, 1e-9));
        QVERIFY(!heightIn(AltitudeFrame::Amsl, 10, 0, kNan).has_value());
        QVERIFY(!heightIn(AltitudeFrame::AboveCurrent, kNan, 0, 0).has_value());
    }
};
QTEST_GUILESS_MAIN(GeoTest)
#include "test_T3_geo.moc"
