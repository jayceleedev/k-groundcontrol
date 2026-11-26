// [0.5.0 T6] specs/features/0.5.0-map.md
#include "DownloadPlan.h"
#include "TileMath.h"
#include <kgc/map/Errors.h>
#include <QtTest>
using kgc::map::MapError;
using kgc::map::planDownload;
namespace {
const QGeoRectangle kKorea(QGeoCoordinate(35.17, 129.15), QGeoCoordinate(35.15, 129.17));
}
class DownloadPlanTest : public QObject {
    Q_OBJECT
private slots:
    /** T6: 범위가 맞고 상한 안이면 받을 타일 수를 돌려줘요. */
    void test_T6_plan_counts_tiles()
    {
        const auto plan = planDownload(kKorea, 15, 17);
        QVERIFY(plan.has_value());
        QCOMPARE(*plan, qint64(113));
    }
    /** T6: 2 ≤ 최소 ≤ 최대 ≤ 20이 아니면 BadZoomRange예요. */
    void test_T6_bad_zoom_range()
    {
        for (const auto& [minZoom, maxZoom] : {std::pair{1, 5}, std::pair{18, 17}, std::pair{15, 21}}) {
            const auto plan = planDownload(kKorea, minZoom, maxZoom);
            QVERIFY(!plan.has_value());
            QCOMPARE(plan.error(), make(MapError::BadZoomRange));
        }
    }
    /** T6: 100,000개를 넘으면(북 38·남 34·서 126·동 130, 10~16은 876,928개) TooManyTiles예요. */
    void test_T6_too_many_tiles()
    {
        const QGeoRectangle wide(QGeoCoordinate(38, 126), QGeoCoordinate(34, 130));
        QCOMPARE(kgc::map::tileCount(wide, 10, 16), qint64(876928));
        const auto plan = planDownload(wide, 10, 16);
        QVERIFY(!plan.has_value());
        QCOMPARE(plan.error(), make(MapError::TooManyTiles));
    }
};
QTEST_APPLESS_MAIN(DownloadPlanTest)
#include "test_T6_download_plan.moc"
