// [0.3.0 T4] specs/features/0.3.0-params.md
#include "ParamMetadata.h"
#include <QtTest>
using namespace kgc::params;
class ParamMetadataTest : public QObject {
    Q_OBJECT
private slots:
    /** T4: 목록형의 모든 필드를 읽어요. */
    void test_T4_flat()
    {
        const auto result = parseFlatMetadata(R"({"parameters":[{"name":"BAT1_N_CELLS","shortDesc":"Cells","longDesc":"Number of cells",
            "units":"S","min":1,"max":16,"default":4,"group":"Battery","rebootRequired":true,
            "values":[{"value":0,"description":"Off"},{"value":1.5,"description":"Half"}],
            "bitmask":[{"index":2,"description":"Bit two"}]}]})");
        QCOMPARE(result.size(), 1);
        const auto meta = result.value("BAT1_N_CELLS");
        QCOMPARE(meta.title, QString("Cells"));
        QCOMPARE(meta.description, QString("Number of cells"));
        QCOMPARE(meta.units, QString("S"));
        QCOMPARE(meta.group, QString("Battery"));
        QCOMPARE(meta.min, 1.0);
        QCOMPARE(meta.max, 16.0);
        QCOMPARE(meta.defaultValue, 4.0);
        QVERIFY(meta.rebootRequired);
        QCOMPARE(meta.options.size(), 2);
        QCOMPARE(meta.options[1].value, 1.5);
        QCOMPARE(meta.options[1].label, QString("Half"));
        QCOMPARE(meta.bits[0].bit, 2);
        QCOMPARE(meta.bits[0].label, QString("Bit two"));
    }
    /** T4: 묶음형의 모든 필드를 읽고 실수 선택지는 값 순서로, json 키는 건너뛰어요. */
    void test_T4_grouped()
    {
        const auto result = parseGroupedMetadata(R"({"json":{"version":0},"BATT_":{"BATT_CAPACITY":{
            "DisplayName":"Capacity","Description":"Pack capacity","Units":"mAh","Range":{"low":"0.5","high":"100"},
            "Values":{"10":"Ten","2.5":"Low"},"Bitmask":{"3":"Three","1":"One"},"RebootRequired":"True","ReadOnly":"True"}},
            "Lua Script":{"SCR_X":{"RebootRequired":"False"}}})");
        QCOMPARE(result.size(), 2);
        QVERIFY(!result.contains("version"));
        const auto meta = result.value("BATT_CAPACITY");
        QCOMPARE(meta.group, QString("BATT_"));
        QCOMPARE(meta.title, QString("Capacity"));
        QCOMPARE(meta.description, QString("Pack capacity"));
        QCOMPARE(meta.units, QString("mAh"));
        QCOMPARE(meta.min, 0.5);
        QCOMPARE(meta.max, 100.0);
        QVERIFY(!meta.defaultValue.has_value());
        QCOMPARE(meta.options[0].value, 2.5);
        QCOMPARE(meta.options[0].label, QString("Low"));
        QCOMPARE(meta.options[1].value, 10.0);
        QCOMPARE(meta.bits[0].bit, 1);
        QCOMPARE(meta.bits[1].label, QString("Three"));
        QVERIFY(meta.rebootRequired);
        QVERIFY(meta.readOnly);
        QVERIFY(!result.value("SCR_X").rebootRequired);
        QCOMPARE(result.value("SCR_X").group, QString("Lua Script"));
    }
    /** T4: 깨진 JSON은 빈 결과예요. */
    void test_T4_broken_json()
    {
        QVERIFY(parseFlatMetadata("{not json").isEmpty());
        QVERIFY(parseGroupedMetadata("[1,").isEmpty());
    }
};
QTEST_APPLESS_MAIN(ParamMetadataTest)
#include "test_T4_param_metadata.moc"
