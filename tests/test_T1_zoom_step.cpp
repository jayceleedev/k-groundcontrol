// [0.5.1 T1] specs/features/0.5.1-map-sharp.md
#include "ZoomStep.h"
#include <QtTest>
using kgc::map::pinchSnapTarget;
using kgc::map::tilePixelRatioFor;
using kgc::map::wheelStep;
using kgc::map::zoomStepTarget;
namespace {
// 시험 창(지도 1440×1000)에서 Qt가 지도 폭에 맞춰 정하는 가장 작은 확대예요.
constexpr double kMin = 2.4919;
constexpr double kMax = 20;
// "오래전에 층을 넘김"을 뜻하는 지난 시간(ms)이에요.
constexpr qint64 kLongAgo = 1000000000;
}
class ZoomStepTest : public QObject {
    Q_OBJECT
private:
    // 휠 사건 하나의 결과가 {steps, sum, fine}과 같은지 봐요.
    static void expectWheel(double sum, bool fine, int delta, qint64 since, int steps, double nextSum, bool nextFine)
    {
        const auto result = wheelStep(sum, fine, delta, since);
        QCOMPARE(result.steps, steps);
        QCOMPARE(result.sum, nextSum);
        QCOMPARE(result.fine, nextFine);
    }
private slots:
    /** T1: 마우스 칸(120의 배수)은 120마다 한 층이고, 방향이 바뀌면 모은 값을 버려요. */
    void test_T1_wheel_notches()
    {
        expectWheel(0, false, 120, kLongAgo, 1, 0, false);
        expectWheel(0, false, 240, kLongAgo, 2, 0, false);
        expectWheel(0, false, -120, kLongAgo, -1, 0, false);
        expectWheel(40, false, 120, kLongAgo, 1, 40, false);
        expectWheel(-40, false, 120, kLongAgo, 1, 0, false);
    }
    /** T1: 잔 값이 한 번이라도 오면 잔 값 흐름이고, 한 층을 넘긴 뒤 130 ms 안에는 넘친 값을 한 칸에서 잘라요. */
    void test_T1_wheel_fine_stream()
    {
        expectWheel(0, false, 15, kLongAgo, 0, 15, true);
        expectWheel(105, true, 15, kLongAgo, 1, 0, true);
        expectWheel(100, true, 30, 50, 0, 120, true);
        expectWheel(120, true, 30, 200, 1, 30, true);
        expectWheel(60, true, -15, kLongAgo, 0, -15, true);
        expectWheel(50, true, 0, kLongAgo, 0, 50, true);
        expectWheel(100, true, 120, 50, 0, 120, true);
        expectWheel(120, true, 120, 200, 1, 120, true);
    }
    /** T1: 시계가 뒤로 가 지난 시간이 음수면 아주 오래전으로 봐요. */
    void test_T1_wheel_clock_back()
    {
        expectWheel(0, false, 15, -5, 0, 15, true);
        expectWheel(110, true, 15, -5, 1, 5, true);
    }
    /** T1: (0, 거짓)에서 15를 여덟 번(사이 10⁹ ms) 넣으면 여덟 번째에만 한 층이에요. */
    void test_T1_wheel_eight_small()
    {
        double sum = 0;
        bool fine = false;
        for (int i = 1; i <= 8; ++i) {
            const auto result = wheelStep(sum, fine, 15, kLongAgo);
            QCOMPARE(result.steps, i == 8 ? 1 : 0);
            sum = result.sum;
            fine = result.fine;
        }
    }
    /** T1: 확대·축소는 그 방향의 다음 정수 층이고, 범위(아래는 소수일 수 있음)로 잘라요. */
    void test_T1_zoom_step_target()
    {
        QCOMPARE(zoomStepTarget(14.6, 1, kMin, kMax), 15.0);
        QCOMPARE(zoomStepTarget(14.6, -1, kMin, kMax), 14.0);
        QCOMPARE(zoomStepTarget(15, 1, kMin, kMax), 16.0);
        QCOMPARE(zoomStepTarget(15, -1, kMin, kMax), 14.0);
        QCOMPARE(zoomStepTarget(15, 2, kMin, kMax), 17.0);
        QCOMPARE(zoomStepTarget(15, 0, kMin, kMax), 15.0);
        QCOMPARE(zoomStepTarget(14.9999999, 1, kMin, kMax), 16.0);
        QCOMPARE(zoomStepTarget(15.0000001, -1, kMin, kMax), 14.0);
        QCOMPARE(zoomStepTarget(3, -1, kMin, kMax), kMin);
        QCOMPARE(zoomStepTarget(kMin, -1, kMin, kMax), kMin);
        QCOMPARE(zoomStepTarget(kMin, 1, kMin, kMax), 3.0);
        QCOMPARE(zoomStepTarget(20, 1, kMin, kMax), 20.0);
        QCOMPARE(zoomStepTarget(19.5, 1, kMin, kMax), 20.0);
    }
    /** T1: 핀치를 놓으면 가까운 층이고, 0.3층 넘게 움직였는데 시작 층으로 돌아가면 움직인 방향으로 한 층이에요. */
    void test_T1_pinch_snap_target()
    {
        QCOMPARE(pinchSnapTarget(15, 15.72, kMin, kMax), 16.0);
        QCOMPARE(pinchSnapTarget(15, 15.2, kMin, kMax), 15.0);
        QCOMPARE(pinchSnapTarget(15, 15.35, kMin, kMax), 16.0);
        QCOMPARE(pinchSnapTarget(15, 15.5, kMin, kMax), 16.0);
        QCOMPARE(pinchSnapTarget(15, 14.6, kMin, kMax), 14.0);
        QCOMPARE(pinchSnapTarget(15, 14.8, kMin, kMax), 15.0);
        QCOMPARE(pinchSnapTarget(15, 14.5, kMin, kMax), 14.0);
        QCOMPARE(pinchSnapTarget(15, 16.6, kMin, kMax), 17.0);
        QCOMPARE(pinchSnapTarget(15, 13.4, kMin, kMax), 13.0);
        QCOMPARE(pinchSnapTarget(19, 20, kMin, kMax), 20.0);
        QCOMPARE(pinchSnapTarget(3, 2.6, kMin, kMax), kMin);
    }
    /** T1: 화면 배율 1.15 이상만 2배 타일이에요. */
    void test_T1_tile_pixel_ratio()
    {
        QCOMPARE(tilePixelRatioFor(1.0), 1);
        QCOMPARE(tilePixelRatioFor(1.05), 1);
        QCOMPARE(tilePixelRatioFor(1.1), 1);
        QCOMPARE(tilePixelRatioFor(1.14), 1);
        QCOMPARE(tilePixelRatioFor(1.15), 2);
        QCOMPARE(tilePixelRatioFor(1.198), 2);
        QCOMPARE(tilePixelRatioFor(1.25), 2);
        QCOMPARE(tilePixelRatioFor(1.5), 2);
        QCOMPARE(tilePixelRatioFor(2.0), 2);
        QCOMPARE(tilePixelRatioFor(3.0), 2);
    }
};
QTEST_APPLESS_MAIN(ZoomStepTest)
#include "test_T1_zoom_step.moc"
