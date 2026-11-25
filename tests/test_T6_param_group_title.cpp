// [0.4.2 T6] specs/features/0.4.2-param-tree.md
#include <kgc/comm/mavlink/Mavlink.h>
#include <kgc/vehicle/firmware/FirmwarePluginFactory.h>
#include <QtTest>
using kgc::vehicle::firmware::FirmwarePluginFactory;
class ParamGroupTitleTest : public QObject {
    Q_OBJECT
private slots:
    /** T6: ArduPilot은 첫 단계 접두어 끝의 숫자를 모두 뗀 뒤 표에서 제목을 찾아요. 표에 없으면 빈 값이에요. */
    void test_T6_ardupilot_strips_digits()
    {
        const auto plugin = FirmwarePluginFactory::create(MAV_AUTOPILOT_ARDUPILOTMEGA);
        QCOMPARE(plugin->paramGroupTitle("BATT"), QString::fromUtf8("배터리"));
        QCOMPARE(plugin->paramGroupTitle("BATT2"), QString::fromUtf8("배터리"));
        QCOMPARE(plugin->paramGroupTitle("EK3"), QString::fromUtf8("위치 추정 필터"));
        QCOMPARE(plugin->paramGroupTitle("SERVO5"), QString::fromUtf8("서보 출력"));
        QVERIFY(plugin->paramGroupTitle("SIM").isEmpty());
    }
    /** T6: PX4와 범용 플러그인은 어떤 접두어에도 제목이 없어요. */
    void test_T6_px4_and_generic_empty()
    {
        for (const std::uint8_t autopilot : {std::uint8_t{MAV_AUTOPILOT_PX4}, std::uint8_t{250}}) {
            const auto plugin = FirmwarePluginFactory::create(autopilot);
            for (const char* prefix : {"BATT", "BATT2", "EK3", "SERVO5", "SIM", "EKF2", "MPC"})
                QVERIFY(plugin->paramGroupTitle(QString::fromLatin1(prefix)).isEmpty());
        }
    }
};
QTEST_APPLESS_MAIN(ParamGroupTitleTest)
#include "test_T6_param_group_title.moc"
