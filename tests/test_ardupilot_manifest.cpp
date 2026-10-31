// [0.2.0 T9] specs/features/0.2.0-fcflash.md
#include "sources/ArduPilotManifest.h"
#include <QtTest>
using kgc::fcFlash::ArduPilotVehicle;
using kgc::fcFlash::ManifestEntry;
using kgc::fcFlash::manifestCandidates;
using kgc::fcFlash::manifestVersions;
using kgc::fcFlash::scanManifest;
namespace {
/** 손으로 만든 표본을 읽어요. 문자열 안의 중괄호와 따옴표, 중첩 객체는 항목을 자르는 코드가 걸려 넘어지기 쉬운 곳이에요. */
QByteArray sampleManifest()
{
    QFile file(QStringLiteral(KGC_TEST_DATA "/ardupilot-manifest.json"));
    return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}
QStringList platforms(const QList<ManifestEntry>& entries)
{
    QStringList names;
    for (const auto& entry : entries) names << entry.platform;
    names.sort();
    return names;
}
}
class ArduPilotManifestTest : public QObject {
    Q_OBJECT
private slots:
    /** T9: Heli는 vehicletype이 Copter여도 mav-type으로 Copter와 갈라요. */
    void test_T9_separates_copter_and_heli()
    {
        const auto entries = scanManifest(sampleManifest(), 1013);
        QCOMPARE(platforms(manifestCandidates(entries, ArduPilotVehicle::Heli, "OFFICIAL")),
                 QStringList({"MatekH743-heli"}));
        QCOMPARE(platforms(manifestCandidates(entries, ArduPilotVehicle::Copter, "OFFICIAL")),
                 QStringList({"MatekH743", "MatekH743-bdshot"}));
    }
    /** T9: 항목의 url과 버전을 그대로 옮겨요. */
    void test_T9_keeps_url_and_version()
    {
        const auto entries = scanManifest(sampleManifest(), 1013);
        const auto heli = manifestCandidates(entries, ArduPilotVehicle::Heli, "OFFICIAL");
        QCOMPARE(heli.size(), 1);
        QCOMPARE(heli.first().url, QString("https://firmware.ardupilot.org/Copter/stable/MatekH743-heli/arducopter-heli.apj"));
        QCOMPARE(heli.first().version, QString("4.7.2"));
    }
    /** T9: 버전 콤보는 OFFICIAL 다음 STABLE-x.y.z이고, 수로 비교해 4.10이 4.9보다 앞이에요. */
    void test_T9_versions_official_then_stable_newest_first()
    {
        const auto versions = manifestVersions(scanManifest(sampleManifest(), 1013), ArduPilotVehicle::Copter);
        QCOMPARE(versions.size(), 4);
        QCOMPARE(versions[0].buildType, QString("OFFICIAL"));
        QCOMPARE(versions[0].version, QString("4.7.2"));
        QCOMPARE(versions[1].buildType, QString("STABLE-4.10.0"));
        QCOMPARE(versions[2].buildType, QString("STABLE-4.9.1"));
        QCOMPARE(versions[3].buildType, QString("STABLE-4.6.3"));
    }
    /** T9: BETA와 DEV는 고르지 않아요. */
    void test_T9_excludes_beta_and_dev()
    {
        const auto entries = scanManifest(sampleManifest(), 1013);
        for (const auto& entry : entries) {
            QVERIFY(entry.buildType == "OFFICIAL" || entry.buildType.startsWith("STABLE-"));
            QVERIFY(!entry.url.contains("/beta/") && !entry.url.contains("/latest/"));
        }
        QVERIFY(!entries.isEmpty());
    }
    /** T9: apj가 아닌 형식은 빼요. */
    void test_T9_excludes_other_formats()
    {
        const auto entries = scanManifest(sampleManifest(), 1013);
        QVERIFY(!entries.isEmpty());
        for (const auto& entry : entries) QVERIFY(entry.url.endsWith(".apj"));
    }
    /** T9: 다섯 기체 밖의 항목은 빼요. */
    void test_T9_excludes_other_vehicles()
    {
        const auto entries = scanManifest(sampleManifest(), 1013);
        QVERIFY(!entries.isEmpty());
        for (const auto& entry : entries) QVERIFY(!entry.url.contains("antennatracker"));
        QCOMPARE(manifestCandidates(entries, ArduPilotVehicle::Plane, "OFFICIAL").size(), 1);
        QCOMPARE(manifestCandidates(entries, ArduPilotVehicle::Rover, "OFFICIAL").size(), 1);
        QCOMPARE(manifestCandidates(entries, ArduPilotVehicle::Sub, "OFFICIAL").size(), 1);
    }
    /** T9: 다른 보드의 항목은 빼요. */
    void test_T9_filters_by_board()
    {
        const auto entries = scanManifest(sampleManifest(), 140);
        QCOMPARE(entries.size(), 1);
        QCOMPARE(entries.first().platform, QString("CubeOrange"));
    }
    /** T9: 보드 255는 9로 봐요. */
    void test_T9_board_255_means_9()
    {
        const auto entries = scanManifest(sampleManifest(), 255);
        QCOMPARE(entries.size(), 1);
        QCOMPARE(entries.first().platform, QString("fmuv3"));
    }
    /** T9: 맞는 보드가 없거나 입력이 깨졌으면 빈 목록이에요. */
    void test_T9_empty_results()
    {
        QVERIFY(scanManifest(sampleManifest(), 4242).isEmpty());
        QVERIFY(scanManifest("not json", 1013).isEmpty());
        QVERIFY(scanManifest({}, 1013).isEmpty());
    }
};
QTEST_APPLESS_MAIN(ArduPilotManifestTest)
#include "test_ardupilot_manifest.moc"
