// [0.0.3 T10] specs/features/0.0.3-windows.md
#include <kgc/secondPanel/CellArrangement.h>
#include <QtTest>
using namespace kgc::secondPanel;
class CellArrangementTest : public QObject {
    Q_OBJECT
private slots:
    /** T10: 일곱 배치의 번호 순서와 빈틈·겹침 없는 전체 영역을 확인해요. */
    void test_T10_rectangles()
    {
        using A = settings::Arrangement;
        const QList<std::pair<A, QList<QRectF>>> fixtures{
            {A::Single, {{0, 0, 1, 1}}},
            {A::Split2h, {{0, 0, .5, 1}, {.5, 0, .5, 1}}},
            {A::Split2v, {{0, 0, 1, .5}, {0, .5, 1, .5}}},
            {A::Main3, {{0, 0, 2.0/3, 1}, {2.0/3, 0, 1.0/3, .5}, {2.0/3, .5, 1.0/3, .5}}},
            {A::Grid2x2, {{0, 0, .5, .5}, {.5, 0, .5, .5}, {0, .5, .5, .5}, {.5, .5, .5, .5}}},
            {A::Grid3x2, {{0, 0, 1.0/3, .5}, {1.0/3, 0, 1.0/3, .5}, {2.0/3, 0, 1.0/3, .5},
                         {0, .5, 1.0/3, .5}, {1.0/3, .5, 1.0/3, .5}, {2.0/3, .5, 1.0/3, .5}}},
            {A::Grid2x3, {{0, 0, .5, 1.0/3}, {.5, 0, .5, 1.0/3}, {0, 1.0/3, .5, 1.0/3},
                         {.5, 1.0/3, .5, 1.0/3}, {0, 2.0/3, .5, 1.0/3}, {.5, 2.0/3, .5, 1.0/3}}}
        };
        for (const auto& [arrangement, expected] : fixtures) {
            const auto rects = cellRects(arrangement);
            QCOMPARE(rects.size(), expected.size());
            double area = 0;
            for (int i = 0; i < rects.size(); ++i) {
                QVERIFY(qAbs(rects[i].x() - expected[i].x()) < 1e-9);
                QVERIFY(qAbs(rects[i].y() - expected[i].y()) < 1e-9);
                QVERIFY(qAbs(rects[i].width() - expected[i].width()) < 1e-9);
                QVERIFY(qAbs(rects[i].height() - expected[i].height()) < 1e-9);
                area += rects[i].width() * rects[i].height();
                for (int j = 0; j < i; ++j) {
                    const auto overlap = rects[i].intersected(rects[j]);
                    QVERIFY(overlap.width() * overlap.height() < 1e-9);
                }
            }
            QVERIFY(qAbs(area - 1) < 1e-9);
        }
    }
};
QTEST_GUILESS_MAIN(CellArrangementTest)
#include "test_cell_arrangement.moc"
