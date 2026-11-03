// [0.4.0 T1] specs/features/0.4.0-sitl.md
#include "SitlSources.h"
#include <QtTest>
using kgc::sitl::SitlPlatform;
using kgc::sitl::SitlRelease;
using kgc::sitl::SitlVehicle;
using kgc::sitl::sitlFiles;
/** T1: 두 운영체제 × 네 기체 × 세 배포판, 24가지의 내려받을 주소와 저장 이름이 스펙 §6.2와 같은지 확인해요.
 * 입력: 스펙 §6.2를 그대로 옮긴 기대값이에요.
 * 출력: 시험 결과예요.
 * 실패: 주소(FullyEncoded)나 저장 이름, 개수가 다르면 실패해요.
 */
class SitlFilesTest : public QObject {
    Q_OBJECT
    const QList<SitlVehicle> vehicles{SitlVehicle::Copter, SitlVehicle::Heli, SitlVehicle::Plane, SitlVehicle::Rover};
    const QList<SitlRelease> releases{SitlRelease::Stable, SitlRelease::Beta, SitlRelease::Latest};
    const QStringList vehicleNames{"copter", "heli", "plane", "rover"};
    const QStringList releaseNames{"stable", "beta", "latest"};
    const QStringList dlls{"cygatomic-1.dll", "cyggcc_s-1.dll", "cyggcc_s-seh-1.dll", "cyggomp-1.dll", "cygiconv-2.dll",
                           "cygintl-8.dll", "cygquadmath-0.dll", "cygssp-0.dll", "cygstdc++-6.dll", "cygwin1.dll"};
    static QString params(int vehicle)
    {
        static const QStringList paths{"default_params/copter.parm", "default_params/copter-heli.parm",
                                       "models/plane.parm", "default_params/rover.parm"};
        return "https://raw.githubusercontent.com/ArduPilot/ardupilot/master/Tools/autotest/" + paths[vehicle];
    }
private slots:
    /** T1: Windows는 실행 파일·DLL 10개·defaults.parm 12개이고, 주소의 +는 %2B예요. */
    void test_T1_windows_files()
    {
        const QStringList stems{"ArduCopter", "ArduHeli", "ArduPlane", "ArduRover"};
        const QStringList stable{"CopterStable/", "CopterStable/", "PlaneStable/", "RoverStable/"};
        for (int v = 0; v < 4; ++v) {
            for (int r = 0; r < 3; ++r) {
                const QString folder = r == 0 ? stable[v] : r == 1 ? QStringLiteral("Beta/") : QString{};
                const QString base = "https://firmware.ardupilot.org/Tools/MissionPlanner/sitl/" + folder;
                const auto files = sitlFiles(SitlPlatform::Windows, vehicles[v], releases[r]);
                QCOMPARE(files.size(), 12);
                QCOMPARE(files.first().url.toString(QUrl::FullyEncoded), base + stems[v] + ".elf");
                QCOMPARE(files.first().fileName, stems[v] + ".exe");
                for (int i = 0; i < dlls.size(); ++i) {
                    QCOMPARE(files[i + 1].fileName, dlls[i]);
                    QCOMPARE(files[i + 1].url.toString(QUrl::FullyEncoded), base + QString(dlls[i]).replace("+", "%2B"));
                }
                QCOMPARE(files.last().url.toString(QUrl::FullyEncoded), params(v));
                QCOMPARE(files.last().fileName, QStringLiteral("defaults.parm"));
            }
        }
        const auto copter = sitlFiles(SitlPlatform::Windows, SitlVehicle::Copter, SitlRelease::Stable);
        QCOMPARE(copter[9].url.toString(QUrl::FullyEncoded),
                 QStringLiteral("https://firmware.ardupilot.org/Tools/MissionPlanner/sitl/CopterStable/cygstdc%2B%2B-6.dll"));
    }
    /** T1: Linux는 실행 파일과 defaults.parm 2개이고, 헬리는 Copter 아래 -heli 폴더예요. */
    void test_T1_linux_files()
    {
        const QStringList paths{"Copter/%1/SITL_x86_64_linux_gnu/arducopter", "Copter/%1/SITL_x86_64_linux_gnu-heli/arducopter-heli",
                                "Plane/%1/SITL_x86_64_linux_gnu/arduplane", "Rover/%1/SITL_x86_64_linux_gnu/ardurover"};
        const QStringList names{"arducopter", "arducopter-heli", "arduplane", "ardurover"};
        for (int v = 0; v < 4; ++v) {
            for (int r = 0; r < 3; ++r) {
                const auto files = sitlFiles(SitlPlatform::Linux, vehicles[v], releases[r]);
                QCOMPARE(files.size(), 2);
                QCOMPARE(files.first().url.toString(QUrl::FullyEncoded),
                         "https://firmware.ardupilot.org/" + paths[v].arg(releaseNames[r]));
                QCOMPARE(files.first().fileName, names[v]);
                QCOMPARE(files.last().url.toString(QUrl::FullyEncoded), params(v));
                QCOMPARE(files.last().fileName, QStringLiteral("defaults.parm"));
            }
        }
    }
    /** T1: 캐시 폴더 이름, --model 값, 실행 폴더 이름이 스펙 §5.4 표와 같아요. */
    void test_T1_names()
    {
        for (int v = 0; v < 4; ++v) {
            for (int r = 0; r < 3; ++r) {
                QCOMPARE(kgc::sitl::sitlFolderName(SitlPlatform::Windows, vehicles[v], releases[r]),
                         "windows-" + vehicleNames[v] + "-" + releaseNames[r]);
                QCOMPARE(kgc::sitl::sitlFolderName(SitlPlatform::Linux, vehicles[v], releases[r]),
                         "linux-" + vehicleNames[v] + "-" + releaseNames[r]);
            }
            QCOMPARE(kgc::sitl::sitlRunFolderName(vehicles[v]), vehicleNames[v]);
        }
        QCOMPARE(kgc::sitl::sitlModel(SitlVehicle::Copter), QStringLiteral("+"));
        QCOMPARE(kgc::sitl::sitlModel(SitlVehicle::Heli), QStringLiteral("heli"));
        QCOMPARE(kgc::sitl::sitlModel(SitlVehicle::Plane), QStringLiteral("plane"));
        QCOMPARE(kgc::sitl::sitlModel(SitlVehicle::Rover), QStringLiteral("rover"));
    }
};
QTEST_GUILESS_MAIN(SitlFilesTest)
#include "test_T1_sitl_files.moc"
