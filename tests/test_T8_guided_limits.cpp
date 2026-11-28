// [0.6.0 T8] specs/features/0.6.0-fly.md
#include <kgc/vehicle/safety/GuidedLimits.h>
#include <kgc/safety/Settings.h>
#include <QSettings>
#include <QTemporaryDir>
#include <QtTest>
#include <limits>
using kgc::vehicle::safety::GuidedCheck;
using kgc::vehicle::safety::GuidedLimits;
using kgc::vehicle::safety::checkGuided;
using kgc::vehicle::safety::guidedLimits;
namespace settings = kgc::safety::settings;
class GuidedLimitsTest : public QObject {
    Q_OBJECT
private:
    QTemporaryDir dir_;
private slots:
    /** 사용자 설정 파일을 건드리지 않게, 설정 저장소를 처음 만들기 전에 빈 임시 폴더로 옮겨요. */
    void initTestCase()
    {
        QVERIFY(dir_.isValid());
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, dir_.path());
    }
    /** T8: 빈 설정에서는 기본 한계 1000 m·2 m·121.92 m예요. */
    void test_T8_defaults()
    {
        const auto limits = guidedLimits();
        QCOMPARE(limits.maxDistanceM, 1000.0);
        QCOMPARE(limits.minAltitudeM, 2.0);
        QCOMPARE(limits.maxAltitudeM, 121.92);
    }
    /** T8: 거리를 먼저 보고, NaN은 한계 밖이며 경계값은 받아들여요. */
    void test_T8_check_guided()
    {
        const GuidedLimits limits{1000, 2, 121.92};
        const double nan = std::numeric_limits<double>::quiet_NaN();
        QCOMPARE(checkGuided(limits, 999.995, 30), GuidedCheck::Ok);
        QCOMPARE(checkGuided(limits, 1000, 30), GuidedCheck::Ok);
        QCOMPARE(checkGuided(limits, 1000.755, 30), GuidedCheck::TooFar);
        QCOMPARE(checkGuided(limits, nan, 30), GuidedCheck::TooFar);
        QCOMPARE(checkGuided(limits, 500, 2), GuidedCheck::Ok);
        QCOMPARE(checkGuided(limits, 500, 1.9), GuidedCheck::TooLow);
        QCOMPARE(checkGuided(limits, 500, nan), GuidedCheck::TooLow);
        QCOMPARE(checkGuided(limits, 500, 121.92), GuidedCheck::Ok);
        QCOMPARE(checkGuided(limits, 500, 121.93), GuidedCheck::TooHigh);
        QCOMPARE(checkGuided(limits, 1000.755, 1.9), GuidedCheck::TooFar);
    }
    /** T8: 최저를 최고보다 크게 적으면 guidedLimits가 둘을 바꿔 돌려줘요. */
    void test_T8_swapped_altitudes()
    {
        QVERIFY(settings::setGuidedMinAltitudeM(50));
        QVERIFY(settings::setGuidedMaxAltitudeM(20));
        const auto limits = guidedLimits();
        QCOMPARE(limits.maxDistanceM, 1000.0);
        QCOMPARE(limits.minAltitudeM, 20.0);
        QCOMPARE(limits.maxAltitudeM, 50.0);
    }
    /** T8: 범위 밖 쓰기(최대 거리 5, 최저 고도 0.1)는 거부되고 값은 그대로예요. */
    void test_T8_out_of_range_rejected()
    {
        const double distance = settings::guidedMaxDistanceM();
        const double minimum = settings::guidedMinAltitudeM();
        QVERIFY(!settings::setGuidedMaxDistanceM(5));
        QVERIFY(!settings::setGuidedMinAltitudeM(0.1));
        QCOMPARE(settings::guidedMaxDistanceM(), distance);
        QCOMPARE(settings::guidedMinAltitudeM(), minimum);
    }
};
QTEST_GUILESS_MAIN(GuidedLimitsTest)
#include "test_T8_guided_limits.moc"
