// [0.6.0 T11] specs/features/0.6.0-fly.md
#include "SitlCommand.h"
#include "SitlSources.h"
#include <QtTest>
using kgc::sitl::SitlPlatform;
using kgc::sitl::SitlRelease;
using kgc::sitl::SitlVehicle;
using kgc::sitl::sitlFiles;
namespace settings = kgc::sitl::settings;
/** T11: 앱 SITL의 쿼드플레인이 비행기와 같은 ArduPlane 실행 파일에 쿼드플레인 기본 파라미터를 쓰고,
 *       모델·폴더 이름과 설정 값이 quadplane인지 확인해요.
 * 입력: 스펙 §5.10·§8 T11의 기대값이에요.
 * 출력: 시험 결과예요.
 * 실패: 주소(FullyEncoded)·저장 이름·모델·폴더 이름·인자·설정 값 목록이 하나라도 다르면 실패해요.
 */
class SitlQuadPlaneTest : public QObject {
    Q_OBJECT
private slots:
    /** T11: Linux 쿼드플레인은 비행기와 같은 arduplane을 받고, 둘째 파일은 quadplane.parm을 defaults.parm으로 받아요. */
    void test_T11_linux_files()
    {
        const auto quadPlane = sitlFiles(SitlPlatform::Linux, SitlVehicle::QuadPlane, SitlRelease::Stable);
        const auto plane = sitlFiles(SitlPlatform::Linux, SitlVehicle::Plane, SitlRelease::Stable);
        QCOMPARE(quadPlane.size(), 2);
        QCOMPARE(quadPlane.first().url, plane.first().url);
        QCOMPARE(quadPlane.first().fileName, QStringLiteral("arduplane"));
        QCOMPARE(quadPlane[1].url.toString(QUrl::FullyEncoded),
                 QStringLiteral("https://raw.githubusercontent.com/ArduPilot/ardupilot/master/Tools/autotest/default_params/quadplane.parm"));
        QCOMPARE(quadPlane[1].fileName, QStringLiteral("defaults.parm"));
    }
    /** T11: Windows 쿼드플레인의 첫 파일은 PlaneStable의 ArduPlane.elf를 ArduPlane.exe로 받아요. */
    void test_T11_windows_executable()
    {
        const auto files = sitlFiles(SitlPlatform::Windows, SitlVehicle::QuadPlane, SitlRelease::Stable);
        QCOMPARE(files.first().url.toString(QUrl::FullyEncoded),
                 QStringLiteral("https://firmware.ardupilot.org/Tools/MissionPlanner/sitl/PlaneStable/ArduPlane.elf"));
        QCOMPARE(files.first().fileName, QStringLiteral("ArduPlane.exe"));
    }
    /** T11: 모델과 캐시 폴더 이름은 quadplane이고, 명령줄에 --model quadplane이 잇달아 있어요. */
    void test_T11_model_folder_arguments()
    {
        QCOMPARE(kgc::sitl::sitlModel(SitlVehicle::QuadPlane), QStringLiteral("quadplane"));
        QCOMPARE(kgc::sitl::sitlFolderName(SitlPlatform::Linux, SitlVehicle::QuadPlane, SitlRelease::Beta),
                 QStringLiteral("linux-quadplane-beta"));
        const auto arguments = kgc::sitl::sitlArguments(SitlVehicle::QuadPlane, {35.1587, 129.1604, 3, 0}, 1, false);
        const auto model = arguments.indexOf(QStringLiteral("--model"));
        QVERIFY(model >= 0);
        QVERIFY(model + 1 < arguments.size());
        QCOMPARE(arguments[model + 1], QStringLiteral("quadplane"));
    }
    /** T11: 설정 sitl.vehicle의 값 목록이 copter·heli·plane·quadplane·rover이고, 쿼드플레인은 설정 값과 오가요. */
    void test_T11_setting_values()
    {
        QCOMPARE(settings::vehicleValues(), (QStringList{"copter", "heli", "plane", "quadplane", "rover"}));
        // 생성된 toString(Vehicle)이 QString을 돌려줘 QCOMPARE의 출력과 겹치므로 ==로 비교해요.
        QVERIFY(kgc::sitl::sitlVehicleToSetting(SitlVehicle::QuadPlane) == settings::Vehicle::Quadplane);
        QCOMPARE(kgc::sitl::sitlVehicleFromSetting(settings::Vehicle::Quadplane), SitlVehicle::QuadPlane);
        QCOMPARE(settings::toString(settings::Vehicle::Quadplane), QStringLiteral("quadplane"));
    }
};
QTEST_GUILESS_MAIN(SitlQuadPlaneTest)
#include "test_T11_sitl_quadplane.moc"
