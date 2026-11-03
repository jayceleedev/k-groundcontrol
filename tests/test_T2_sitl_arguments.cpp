// [0.4.0 T2] specs/features/0.4.0-sitl.md
#include "SitlCommand.h"
#include <QtTest>
#include <clocale>
using kgc::sitl::SitlHome;
using kgc::sitl::SitlVehicle;
using kgc::sitl::sitlArguments;
/** T2: 기본 홈으로 만든 명령줄이 스펙 §6.3과 같고, 독일어 환경에서도 소수점이 .인지 확인해요.
 * 입력: 해운대 기본 홈(35.1587, 129.1604, 3, 0), 속도 배수 1이에요.
 * 출력: 시험 결과예요.
 * 실패: 인자 목록이 한 글자라도 다르면 실패해요.
 */
class SitlArgumentsTest : public QObject {
    Q_OBJECT
    const SitlHome home{35.1587, 129.1604, 3, 0};
    const QStringList expected{"--model", "+", "--home", "35.1587000,129.1604000,3.0,0", "--speedup", "1", "-I0",
                               "--serial0", "udpclient:127.0.0.1:14550", "--defaults", "defaults.parm"};
private slots:
    /** T2: 값마다 따로인 인자이고 따옴표가 없어요. */
    void test_T2_default_home()
    {
        QCOMPARE(sitlArguments(SitlVehicle::Copter, home, 1, false), expected);
    }
    /** T2: wipe가 참이면 끝에 --wipe가 붙어요. */
    void test_T2_wipe_last()
    {
        QCOMPARE(sitlArguments(SitlVehicle::Copter, home, 1, true), QStringList(expected) << "--wipe");
    }
    /** T2: 다른 기체는 --model 값만 달라지고, 속도 배수와 다른 홈도 그대로 들어가요. */
    void test_T2_other_values()
    {
        const auto arguments = sitlArguments(SitlVehicle::Heli, {37.5665, 126.978, 30, 90}, 4, false);
        QCOMPARE(arguments[1], QStringLiteral("heli"));
        QCOMPARE(arguments[3], QStringLiteral("37.5665000,126.9780000,30.0,90"));
        QCOMPARE(arguments[5], QStringLiteral("4"));
    }
    /** T2: 시스템 언어를 독일어(de_DE)로 둬도 소수점은 .이에요. */
    void test_T2_german_locale()
    {
        QLocale::setDefault(QLocale(QLocale::German, QLocale::Germany));
        std::setlocale(LC_ALL, "de_DE.UTF-8");
        QCOMPARE(QLocale().toString(1.5), QStringLiteral("1,5"));
        QCOMPARE(sitlArguments(SitlVehicle::Copter, home, 1, false), expected);
        QLocale::setDefault(QLocale::c());
        std::setlocale(LC_ALL, "C");
    }
};
QTEST_GUILESS_MAIN(SitlArgumentsTest)
#include "test_T2_sitl_arguments.moc"
