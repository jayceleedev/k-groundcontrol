// [0.0.3 T11] specs/features/0.0.3-windows.md
#include <kgc/ui/WindowPlacement.h>
#include <QtTest>
using namespace kgc::ui;
class WindowPlacementTest : public QObject {
    Q_OBJECT
private slots:
    /** T11: 화면 기준 좌표와 제목 여유, 고장 난 저장값의 복원을 확인해요. */
    void test_T11_restore_data()
    {
        QTest::addColumn<QString>("saved");
        QTest::addColumn<int>("screen");
        QTest::addColumn<QRect>("geometry");
        QTest::newRow("second") << "second|100|80|600|400" << 1 << QRect(2020, 180, 600, 400);
        QTest::newRow("outside") << "second|-50|900|600|400" << 1 << QRect(1920, 500, 600, 400);
        QTest::newRow("oversized") << "second|90|20|2000|1000" << 1 << QRect(1920, 132, 1280, 768);
        QTest::newRow("missing") << "missing|100|80|600|400" << 0 << QRect(480, 240, 960, 600);
        QTest::newRow("empty") << "" << 0 << QRect(480, 240, 960, 600);
        QTest::newRow("broken") << "primary|no|0|1|2" << 0 << QRect(480, 240, 960, 600);
        QTest::newRow("negative") << "primary|0|0|-1|600" << 0 << QRect(480, 240, 960, 600);
        QTest::newRow("title") << "second|100|10|600|400" << 1 << QRect(2020, 132, 600, 400);
    }
    void test_T11_restore()
    {
        QFETCH(QString, saved);
        QFETCH(int, screen);
        QFETCH(QRect, geometry);
        const auto result = restorePlacement(saved,
            {{"primary", {0, 0, 1920, 1080}}, {"second", {1920, 100, 1280, 800}}}, {960, 600});
        QCOMPARE(result.screenIndex, screen);
        QCOMPARE(result.geometry, geometry);
    }
    /** T11: 구분자가 든 이름과 중복 이름, 큰 기본 크기도 안전하게 복원해요. */
    void test_T11_names_and_defaults()
    {
        const auto saved = savePlacement("a|b", {50, 40, 600, 400});
        QCOMPARE(saved, "a|b|50|40|600|400");
        const QList<ScreenInfo> screens{{"a|b", {-1280, 0, 1280, 800}}, {"a|b", {0, 0, 1920, 1080}}};
        const auto same = restorePlacement(saved, screens, {960, 600});
        QCOMPARE(same.screenIndex, 0);
        QCOMPARE(same.geometry, QRect(-1230, 40, 600, 400));
        const auto large = restorePlacement({}, screens, {2000, 1200});
        QCOMPARE(large.geometry, QRect(-1280, 32, 1280, 768));
    }
};
QTEST_GUILESS_MAIN(WindowPlacementTest)
#include "test_window_placement.moc"
