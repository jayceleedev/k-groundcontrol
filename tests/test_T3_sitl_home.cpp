// [0.4.0 T3] specs/features/0.4.0-sitl.md
#include "SitlCommand.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtTest>
#include <limits>
using kgc::sitl::SitlHome;
using kgc::sitl::SitlHomeField;
using kgc::sitl::checkHome;
namespace settings = kgc::sitl::settings;
/** T3: checkHome이 경계값은 통과시키고 넘은 칸만 돌려주는지, <이름>Default()가 JSON 기본값과 같은지 확인해요.
 * 입력: 경계값(−90, 90, −180, 180, −500, 9000, 0, 359)과 그 바깥 값, sitl.settings.json이에요.
 * 출력: 시험 결과예요.
 * 실패: 목록이나 기본값이 다르면 실패해요.
 */
class SitlHomeTest : public QObject {
    Q_OBJECT
private slots:
    /** T3: 양 끝 경계값은 빈 목록이에요. */
    void test_T3_boundaries_pass()
    {
        QVERIFY(checkHome({-90, -180, -500, 0}).isEmpty());
        QVERIFY(checkHome({90, 180, 9000, 359}).isEmpty());
        QVERIFY(checkHome({35.1587, 129.1604, 3, 0}).isEmpty());
    }
    /** T3: 경계를 넘은 칸만 목록에 들어가요. */
    void test_T3_only_bad_fields()
    {
        QCOMPARE(checkHome({90.0000001, 0, 0, 0}), QList<SitlHomeField>{SitlHomeField::Latitude});
        QCOMPARE(checkHome({-90.5, 0, 0, 0}), QList<SitlHomeField>{SitlHomeField::Latitude});
        QCOMPARE(checkHome({0, 180.5, 0, 0}), QList<SitlHomeField>{SitlHomeField::Longitude});
        QCOMPARE(checkHome({0, -181, 0, 0}), QList<SitlHomeField>{SitlHomeField::Longitude});
        QCOMPARE(checkHome({0, 0, -500.1, 0}), QList<SitlHomeField>{SitlHomeField::Altitude});
        QCOMPARE(checkHome({0, 0, 9000.1, 0}), QList<SitlHomeField>{SitlHomeField::Altitude});
        QCOMPARE(checkHome({0, 0, 0, -1}), QList<SitlHomeField>{SitlHomeField::Heading});
        QCOMPARE(checkHome({0, 0, 0, 360}), QList<SitlHomeField>{SitlHomeField::Heading});
        const QList<SitlHomeField> all{SitlHomeField::Latitude, SitlHomeField::Longitude, SitlHomeField::Altitude,
                                       SitlHomeField::Heading};
        QCOMPARE(checkHome({95, 200, 10000, 400}), all);
    }
    /** T3: 숫자가 아닌 값(NaN)도 벗어난 칸이에요. */
    void test_T3_nan_is_bad()
    {
        const auto nan = std::numeric_limits<double>::quiet_NaN();
        QCOMPARE(checkHome({nan, 0, nan, 0}), (QList<SitlHomeField>{SitlHomeField::Latitude, SitlHomeField::Altitude}));
    }
    /** T3: 생성된 <이름>Default()가 sitl.settings.json의 default와 같아요. */
    void test_T3_defaults_match_json()
    {
        QFile file(QStringLiteral(KGC_SITL_SETTINGS));
        QVERIFY(file.open(QIODevice::ReadOnly));
        QHash<QString, QJsonValue> defaults;
        for (const auto entry : QJsonDocument::fromJson(file.readAll()).object().value("settings").toArray())
            defaults.insert(entry.toObject().value("id").toString(), entry.toObject().value("default"));
        QCOMPARE(defaults.size(), 7);
        QCOMPARE(settings::toString(settings::vehicleDefault()), defaults.value("sitl.vehicle").toString());
        QCOMPARE(settings::toString(settings::releaseDefault()), defaults.value("sitl.release").toString());
        QCOMPARE(settings::homeLatitudeDefault(), defaults.value("sitl.homeLatitude").toDouble());
        QCOMPARE(settings::homeLongitudeDefault(), defaults.value("sitl.homeLongitude").toDouble());
        QCOMPARE(settings::homeAltitudeDefault(), defaults.value("sitl.homeAltitude").toDouble());
        QCOMPARE(settings::homeHeadingDefault(), defaults.value("sitl.homeHeading").toInt());
        QCOMPARE(settings::speedupDefault(), defaults.value("sitl.speedup").toInt());
    }
};
QTEST_GUILESS_MAIN(SitlHomeTest)
#include "test_T3_sitl_home.moc"
