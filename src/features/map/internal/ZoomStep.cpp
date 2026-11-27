// [0.5.1 5.1 T1] specs/features/0.5.1-map-sharp.md
#include "ZoomStep.h"
#include "Constants.h"
#include <algorithm>
#include <cmath>
#include <limits>
namespace kgc::map {
WheelStep wheelStep(double sum, bool fine, int delta, qint64 sinceStepMs)
{
    if (delta == 0) return {0, sum, fine};
    // 한 번이라도 잔 값이 오면 그 몸짓은 끝까지 터치패드·부드러운 휠로 봐요.
    fine = fine || delta % kWheelNotch != 0;
    if (sum != 0 && (sum > 0) != (delta > 0)) sum = 0;
    sum += delta;
    if (sinceStepMs < 0) sinceStepMs = std::numeric_limits<qint64>::max();
    int steps = 0;
    if (!fine) {
        steps = static_cast<int>(std::trunc(sum / kWheelNotch));
        sum -= steps * kWheelNotch;
    } else if (std::abs(sum) >= kWheelNotch) {
        const int sign = sum > 0 ? 1 : -1;
        if (sinceStepMs >= kWheelStepIntervalMs) {
            steps = sign;
            sum -= steps * kWheelNotch;
        } else {
            // 넘친 값은 버려요. 손을 뗀 뒤에 모아 둔 값으로 층이 더 넘어가지 않아요.
            sum = sign * kWheelNotch;
        }
    }
    return {steps, sum, fine};
}
double zoomStepTarget(double current, int steps, double minZoom, double maxZoom)
{
    double target = current;
    // 핀치가 남긴 14.9999999 같은 값도 정수 층으로 보도록 kZoomEpsilon만큼 여유를 둬요.
    if (steps > 0) target = std::floor(current + kZoomEpsilon) + steps;
    else if (steps < 0) target = std::ceil(current - kZoomEpsilon) + steps;
    return std::max(minZoom, std::min(maxZoom, target));
}
double pinchSnapTarget(double startZoom, double endZoom, double minZoom, double maxZoom)
{
    double target = std::round(endZoom);
    // 일부러 조금 오므리거나 벌린 것이 시작 층으로 되돌아가지 않게 해요.
    if (target == std::round(startZoom) && std::abs(endZoom - startZoom) >= kPinchSnapMin)
        target = zoomStepTarget(startZoom, endZoom > startZoom ? 1 : -1, minZoom, maxZoom);
    return std::max(minZoom, std::min(maxZoom, target));
}
int tilePixelRatioFor(double screenRatio)
{
    return screenRatio >= kHighDpiMinRatio ? 2 : 1;
}
}
