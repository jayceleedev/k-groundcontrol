// [0.2.0 T5] specs/features/0.2.0-fcflash.md
#include <kgc/vehicle/safety/ArmedPolicy.h>
#include <QtTest>
using kgc::app::job::OperationKind;
using kgc::vehicle::safety::Decision;
using kgc::vehicle::safety::decide;
class ArmedPolicyTest : public QObject {
    Q_OBJECT
private slots:
    /** T5: BASE §9.2 표의 Deny 행은 시동 중에 전부 거부해요. */
    void test_T5_deny_rows()
    {
        for (const auto kind : {OperationKind::SetupPage, OperationKind::FcFlash, OperationKind::EscFlash,
                                OperationKind::ElrsFlash, OperationKind::TrackerFlash, OperationKind::EscConfig,
                                OperationKind::ElrsConfig, OperationKind::AiWrite, OperationKind::ParamBulk,
                                OperationKind::LayoutWrite, OperationKind::MotorTest})
            QCOMPARE(decide(kind, true), Decision::Deny);
    }
    /** T5: BASE §9.2 표의 경고 행은 시동 중에 길게 눌러 확인이에요. */
    void test_T5_warning_rows()
    {
        for (const auto kind : {OperationKind::ParamSingle, OperationKind::Tuning, OperationKind::Autotune})
            QCOMPARE(decide(kind, true), Decision::AllowWithWarning);
    }
    /** T5: BASE §9.2 표의 나머지 행은 시동 중에도 허용이에요. */
    void test_T5_allow_rows()
    {
        for (const auto kind : {OperationKind::TrackerConfig, OperationKind::TrackerCalib, OperationKind::TrackerManual,
                                OperationKind::FlyAction, OperationKind::MissionStartStop, OperationKind::Read})
            QCOMPARE(decide(kind, true), Decision::Allow);
    }
    /** T5: 시동이 아니면 어떤 작업도 허용이에요. Deny 행으로 판정이 armed를 보는지도 확인해요. */
    void test_T5_not_armed_allows_all()
    {
        for (const auto kind : {OperationKind::SetupPage, OperationKind::FcFlash, OperationKind::EscFlash,
                                OperationKind::ElrsFlash, OperationKind::TrackerFlash, OperationKind::EscConfig,
                                OperationKind::ElrsConfig, OperationKind::AiWrite, OperationKind::ParamBulk,
                                OperationKind::LayoutWrite, OperationKind::MotorTest, OperationKind::ParamSingle,
                                OperationKind::Tuning, OperationKind::Autotune, OperationKind::TrackerConfig,
                                OperationKind::TrackerCalib, OperationKind::TrackerManual, OperationKind::FlyAction,
                                OperationKind::MissionStartStop, OperationKind::Read})
            QCOMPARE(decide(kind, false), Decision::Allow);
        QVERIFY(decide(OperationKind::FcFlash, true) != decide(OperationKind::FcFlash, false));
    }
};
QTEST_APPLESS_MAIN(ArmedPolicyTest)
#include "test_armed_policy.moc"
