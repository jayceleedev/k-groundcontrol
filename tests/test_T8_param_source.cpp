// [0.3.0 T8] specs/features/0.3.0-params.md
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <QtTest>
using kgc::vehicle::firmware::FirmwarePluginFactory;
using kgc::vehicle::firmware::ParamMetadataFormat;
class ParamSourceTest : public QObject {
    Q_OBJECT
private slots:
    /** T8: PX4는 그 버전 주소 다음에 stable 주소를 시도해요. */
    void test_T8_px4()
    {
        const auto source = FirmwarePluginFactory::create(MAV_AUTOPILOT_PX4)->paramMetadataSource(MAV_TYPE_QUADROTOR, "1.15.2");
        QCOMPARE(source.format, ParamMetadataFormat::Flat);
        QCOMPARE(source.cacheName, QString("PX4-1.15.2"));
        QCOMPARE(source.urls, QList<QUrl>({QUrl("https://px4-travis.s3.amazonaws.com/Firmware/v1.15.2/_general/parameters.json"),
                                           QUrl("https://px4-travis.s3.amazonaws.com/Firmware/stable/_general/parameters.json")}));
    }
    /** T8: ArduPilot은 기체 종류별 폴더를 minor부터 두 단계 아래까지 시도해요. */
    void test_T8_ardupilot()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        const auto source = plugin->paramMetadataSource(MAV_TYPE_QUADROTOR, "4.7.2");
        QCOMPARE(source.format, ParamMetadataFormat::Grouped);
        QCOMPARE(source.cacheName, QString("Copter-4.7"));
        const QString base("https://raw.githubusercontent.com/ArduPilot/ParameterRepository/main/");
        QCOMPARE(source.urls, QList<QUrl>({QUrl(base + "Copter-4.7/apm.pdef.json"), QUrl(base + "Copter-4.6/apm.pdef.json"),
                                           QUrl(base + "Copter-4.5/apm.pdef.json")}));
        QCOMPARE(plugin->paramMetadataSource(MAV_TYPE_DECAROTOR, "4.7.2").cacheName, QString("Copter-4.7"));
        QCOMPARE(plugin->paramMetadataSource(MAV_TYPE_GENERIC, "4.7.2").cacheName, QString("Copter-4.7"));
        QCOMPARE(plugin->paramMetadataSource(MAV_TYPE_FIXED_WING, "4.1.0").urls.size(), 2);
        QCOMPARE(plugin->paramMetadataSource(MAV_TYPE_FIXED_WING, "4.1.0").cacheName, QString("Plane-4.1"));
        QCOMPARE(plugin->paramMetadataSource(MAV_TYPE_VTOL_TILTROTOR, "4.6.0").cacheName, QString("Plane-4.6"));
        QCOMPARE(plugin->paramMetadataSource(MAV_TYPE_SURFACE_BOAT, "4.6.0").cacheName, QString("Rover-4.6"));
        QCOMPARE(plugin->paramMetadataSource(MAV_TYPE_SUBMARINE, "4.6.0").cacheName, QString("Sub-4.6"));
        QCOMPARE(plugin->paramMetadataSource(MAV_TYPE_ANTENNA_TRACKER, "4.6.0").cacheName, QString("Tracker-4.6"));
    }
    /** T8: 모르는 기체 종류와 범용 플러그인은 None이에요. */
    void test_T8_unknown()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        QCOMPARE(plugin->paramMetadataSource(MAV_TYPE_ONBOARD_CONTROLLER, "4.7.2").format, ParamMetadataFormat::None);
        QCOMPARE(FirmwarePluginFactory::create(250)->paramMetadataSource(MAV_TYPE_QUADROTOR, "1.0.0").format, ParamMetadataFormat::None);
    }
};
QTEST_APPLESS_MAIN(ParamSourceTest)
#include "test_T8_param_source.moc"
